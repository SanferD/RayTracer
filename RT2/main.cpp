#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#include "geometry.hpp"
#include "image.hpp"
#include "lights.hpp"
#include "params.hpp"
#include "utils.hpp"
#include "surfaces.hpp"

#define _CLAMP(x) (x>1.0? 1.0: x)
#define CLAMP(c) (Color(_CLAMP(c.r), _CLAMP(c.g), _CLAMP(c.b)))
#define RAND() (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))
#define PI 3.14159265

Params params;
std::vector<Surface*> surfaces;
std::vector<LightSource*> lights;

/**
 * Gets the shadow flag value for the given surface and vector
 * @param  source     The light source
 * @param  shadow_ray The shadow ray
 * @param  L          The direction from intersection to light source
 * @param  surface    The surface object whose shadows we wish to cast
 * @param  intersect  The point of intersection on that surface
 * @return            The shadow value
 */
float get_shadow_flag(LightSource *source, Ray shadow_ray, Vec3& L,
											Surface *surface, Vec3& intersect) {
	float shadow = 1.0;
	for (Surface *s : surfaces) {
		if (s != surface) {
			/* get intersection. skip if no intersection. */
			float alpha = s->hit(shadow_ray);

			if (alpha > 0.0) {
				bool is_valid = true;

				/* for point-source: valid when intersection is before light source */
				if (source->w != 0.0) {
					Vec3 shadow_intersect = shadow_ray(alpha);
					is_valid =  (shadow_intersect - intersect).norm() <= 
			  							(source->p - intersect).norm();
				}

				if (is_valid) {
					shadow = 0.0;
					break;
				}
			}
		}
	}

	return shadow;
}

/**
 * Gets the color using Phong Illumination and shadows
 * @param  r       The ray which itersects the surface
 * @param  t       The parameter value for r at the point of intersection
 * @param  surface The surface which is intersected by r
 * @return         The color for the intersection point
 */
Color get_color(Ray r, float t, Surface *surface) {
	/* setup */
	MtlColor mtlcolor = surface->mtl_color;
	Vec3 center = surface->center, intersect = r(t);
	
	float ka = mtlcolor.ka, kd = mtlcolor.kd, ks = mtlcolor.ks, n = mtlcolor.n;
	Color Od = mtlcolor.Od, Os = mtlcolor.Os;

	/* ambient term */
	Color ret = ka*Od;
	
	for (LightSource *source : lights) {
		/* Calculate N, L, H */
		Vec3 N = (intersect - center).normalize();
		
		Vec3 L;
		if (source->w == 0.0)
			L = -1.0*source->p;
		else
			L = (source->p - intersect).normalize();

		Vec3 V = (params.eye - intersect).normalize();
		Vec3 H = (L + V).normalize();

		/* intensity only relevant if multiple light-sources are present */
		Color intensity = lights.size()==1? Color(1.0, 1.0, 1.0): source->c;
		
		/* compute the defuse and specular term, clamp negative value to 0.0 */
		float d1=N.dot(L), d2=N.dot(H);
		Color diffuse  = kd*Od*(d1<0.0? 0.0: d1);
		Color specular = ks*Os*pow(d2<0.0? 0.0: d2, n);
		
		/* calculate soft shadows. Jitter only makes sense for point sources */
		int bundle_size=1; // use 1 for hard shadows
		float	shadow = get_shadow_flag(	source,
																		Ray(intersect, L, true), 
																		L, 
																		surface,
																		intersect);
		if (source->w != 0.0) {
			for (int i=1; i<bundle_size; i++) {
				Vec3 jitter(RAND(), RAND(), RAND());
				L = (source->p + 2.0*jitter - intersect).normalize();
				shadow += get_shadow_flag(	source,
																		Ray(intersect, L, true), 
																		L, 
																		surface,
																		intersect);
			}
			shadow /= (float) bundle_size;
		}

		/* spotlight: check to see if intersection is in field-of-illumination */
		if (source->w == -1.0) {
			Spotlight *spotlight = (Spotlight*) source;
			if (spotlight->dir.dot(-1.0*L) < cos(spotlight->theta * PI / 180.0))
				intensity = Color(0.0, 0.0, 0.0);
		}

		/* add the diffuse and specular terms */
		Color color = shadow*intensity*( diffuse + specular );
		ret = ret + color;
	}

	/* upper clamp */
	return CLAMP(ret);
}

int main(int argc, char *argv[]) {
	/* Basic input validation */
	if (argc != 2) {
		printf("usage: ./main <input-file>\n");
		exit(1);
	}

	std::string filename(argv[1]);

	/* Get image parameters and initialize the image */
	try {
		params = parse_input(filename, surfaces, lights);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	Image img(params.width, params.height, params.bkg_color);

	/* Create the viewing window */
	ViewWindow vw(params);

	/* Fill the image with the nearest ray intersection points */
	srand(time(NULL)); // seed for soft shadows
	for (Ray ray : vw.all_rays) {
		Surface *s;
		MtlColor mtl_color;
		float alpha_max = INFINITY; 
		
		for (Surface *surface : surfaces) {
			float alpha = surface->hit(ray);
			if (alpha > 0.0 && alpha < alpha_max) {
				alpha_max = alpha;
				mtl_color = surface->mtl_color;
				s = surface;
			}
		}

		if (alpha_max != INFINITY)
			img(ray.r, ray.c) = get_color(ray, alpha_max, s);
	}

	/* save the image */
	std::string fn = remove_ext(filename) + ".ppm";
	img.save(fn);

	/* clean-up */
	for (LightSource *source : lights)
		delete source;
	for (Surface *surface : surfaces)
		delete surface;

	return 0;
}