#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#include "geometry.hpp"
#include "image.hpp"
#include "lights.hpp"
#include "params.hpp"
#include "utils.hpp"
#include "surfaces.hpp"

std::default_random_engine generator;
std::normal_distribution<float> distribution(0.0, 1.0);

#define _CLAMP(x) (x>1.0? 1.0: x)
#define CLAMP(c) (Color(_CLAMP(c.r), _CLAMP(c.g), _CLAMP(c.b)))
#define RAND() (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))
#define RANDN() (distribution(generator))
#define PI 3.14159265
#define OFFSET .02

#define DEFAULT_R_D 5		// Recursive depth for reflections
#define DEFAULT_T_D 5		// Recursive depth for refractions

Params params;
std::vector<Surface*> surfaces;
std::vector<LightSource*> lights;
std::vector<Texture*> textures;

float get_shadow_flag(LightSource *source, Ray shadow_ray, Vec3& L,
											Surface *surface, Vec3& intersect);
Color get_color(Ray r, float t, Surface *surface, bool reflect=true, bool refract=true);
Surface* trace_ray(Ray ray, float &out_alpha, float offset=0.0);
Color reflect_ray(Vec3 intersect, Vec3 I, Vec3 N, float eta, int depth);
Color refract_ray(Vec3 intersect, Vec3 I, Vec3 N, float alpha, float eta, int depth);
float randn() {

}

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
	float shadow = 1.0, diff = 0.0;
	int count = 0;
	for (Surface *s : surfaces) {
		if (s != surface) {
			/* get intersection. skip if no intersection. */
			float dummy, alpha = s->hit(shadow_ray, dummy);

			if (alpha > 0.0) {
				bool is_valid = true;

				/* for point-source: valid when intersection is before light source */
				if (source->w != 0.0) {
					Vec3 shadow_intersect = shadow_ray(alpha);
					is_valid =  (shadow_intersect - intersect).norm() <= 
			  							(source->p - intersect).norm();
				}

				if (is_valid) {
					diff += s->mtl_color.alpha;
					count++;
					// shadow = 0.0;
					// break;
				}
			}
		}
	}

	if (count>0)
		shadow -= diff/(float) count;

	return shadow;
}


/**
 * Refract the ray. Assumes that if a ray enters a surface then it will leave the surface after traversing it for some non-zero time.
 * @param  intersect The point of intersection of the transparent object
 * @param  I         The incident ray
 * @param  N         The normal
 * @param  alpha     The 
 * @param  eta       The refractive index
 * @param  depth     Number of iterations
 * @return           The color
 */
Color refract_ray(Vec3 intersect, Vec3 I, Vec3 N, float alpha, float eta, int depth) {
	Color ret = Color(0.0, 0.0, 0.0);
	float t1, t2, __offset;
	Surface *s;
	for (int i=0; i!=depth && eta>=0.0; i++) {
		/* use the desired normal */
		float cos_val = I.dot(N);
		if (cos_val<0.0) {
			N = -1.0*N;
			cos_val = I.dot(N);
		}

		/* Fresnell coeffs */
		const float F0 = powf((eta-1.0)/(eta+1.0), 2.0);
		const float Fr = F0 + (1.0-F0)*powf(1.0-cos_val, 5.0);

		/* Transmit ray from medium to object */
		float r_eta = 1.0/eta;
		Vec3 T_dir = -1.0*N*sqrt( 1.0 - powf(r_eta, 2.0)*(1-powf(cos_val, 2.0)) ) + r_eta*(cos_val*N-I);
		Ray T1 = Ray(intersect, T_dir.normalize(), true);
		for (	s = trace_ray(T1, t1, __offset=OFFSET); 
					t1<0.001 && s;
					s = trace_ray(T1, t1, __offset+=.001)
				);

		if (!s)
			break;

		/* get new parameters */
		I = (intersect-T1(t1)).normalize();
		intersect = T1(t1);
		N = s->get_normal(intersect);
		
		/* get angle */
		cos_val = I.dot(N);
		if (cos_val < 0.0) {
			N = -1.0*N;
			cos_val = I.dot(N);
		}

		/* transmit ray from object to medium */
		r_eta = 1.0/r_eta;
		T_dir = -1.0*N*sqrt( 1.0 - powf(r_eta, 2.0)*(1-powf(cos_val, 2.0)) ) + r_eta*(cos_val*N-I);
		Ray T2 = Ray(intersect, T_dir.normalize(), true);
		for (	s = trace_ray(T2, t2, __offset=OFFSET);
					t2<0.001 && s;
					s = trace_ray(T1, t2, __offset+=.001)
				);

		if (!s)
			break;
			
		/* add color using beers law for attenuation */
		float beers_law = exp( -1.0*alpha*fabs(t2-t1) );
		ret = ret + (1.0-Fr)*beers_law*get_color(T2, t2, s, true, false);

		/* get values for next iteration */
		I = (intersect - T2(t2)).normalize();
		intersect = T2(t2);
		N = s->get_normal(intersect);
		eta = s->mtl_color.eta;
		alpha = s->mtl_color.alpha;
	}
	return ret;
}


/**
 * The reflects the ray in the scene.
 * @param  intersect The point of intersection of the eye-ray
 * @param  I         The direction of light from intersection to eye
 * @param  N         The surface normal
 * @param  eta       The index of refraction of the intersected object
 * @param  depth     Recursive depth
 * @return           Returns the compound color. Color might overflow but not underflow.
 */
Color reflect_ray(Vec3 intersect, Vec3 I, Vec3 N, float eta, int depth) {
	Color ret = Color(0.0, 0.0, 0.0);
	while (depth-- > 0 && eta>=0.0) {

		/* perform schlick approximation */
		float t, cos_val = I.dot(N);
		if (cos_val < 0.0) {
			N = -1.0*N;
			cos_val = I.dot(N);
		}
		const float F0 = powf((eta-1.0)/(eta+1.0), 2.0);
		const float Fr = F0 + (1.0-F0)*powf(1.0-cos_val, 5.0);
		Vec3 R_dir = 2.0*cos_val*N - I;
		Ray R(intersect, R_dir, true);

		/* get the color of closest reflected surface */
		float __offset;
		Surface *s = trace_ray(R, t, __offset=OFFSET);
		while (t<0.001 && s) {
			s = trace_ray(R, t, __offset+=.001);
		}
		if (!s)
			break;

		/* get ready for next iteration */
		ret = ret + Fr*get_color(R, t, s, false, true);
		I = (intersect - R(t)).normalize();
		intersect = R(t);
		N = s->get_normal(intersect);
		eta = s->mtl_color.eta;
	}

	return CLAMP(ret);
}




/**
 * Gets the color using Phong Illumination and shadows
 * @param  r       		The ray which itersects the surface
 * @param  t       		The parameter value for r at the point of intersection
 * @param  surface 		The surface which is intersected by r
 * @param  reflect 		Flag to decide if to calculate reflections. Default is true.
 * @param  refract 		Flag to decide if to calculate refractions. Default is true.
 * @return         		The color for the intersection point
 */
Color get_color(Ray r, float t, Surface *surface, bool reflect, bool refract) {
	/* setup */
	MtlColor mtlcolor = surface->mtl_color;
	Vec3 intersect = r(t);
	
	float ka = mtlcolor.ka, kd = mtlcolor.kd, ks = mtlcolor.ks, n = mtlcolor.n;
	Color Os = mtlcolor.Os, Od; float u, v;
	if (surface->t_idx == -1)
		Od = mtlcolor.Od;
	else {
		Texture *t = textures[surface->t_idx];
		u = surface->get_u(intersect);
		v = surface->get_v(intersect);
		Od = t->operator()(u, v);
	}

	/* ambient term */
	Color ret = ka*Od;
	
	for (LightSource *source : lights) {
		/* Calculate N, L, H */
		Vec3 N = surface->get_normal(intersect);
		
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
		intersect = r(t);
		Vec3 I = (params.eye - intersect).normalize();
		float eta = mtlcolor.eta, alpha = mtlcolor.alpha;
		if (surface->type==3) {
			Triangle T = *(Triangle *) surface;
			N = Vec3(T.A, T.B, T.C).normalize();
		}

		if (reflect)
			ret = ret + reflect_ray(intersect, I, N, eta, DEFAULT_R_D);
		if (refract)
			ret = ret	+ refract_ray(intersect, I, N, alpha, eta, DEFAULT_T_D);
	}

	/* upper clamp */
	return CLAMP(ret);
}

/**
 * Traces the ray and tries to find the closest intersecting surface.
 * @param  ray         The ray to trace
 * @param  out_alpha   The alpha value such that r(alpha) is the intersection point.
 * @param  out_surface The closest surface which the ray intersects, or NULL if none.
 */
Surface* trace_ray(Ray ray, float &out_alpha, float offset) {
	ray.org = ray(offset);
	float out_alpha1, out_alpha2;
	out_alpha1 = INFINITY; 
	Surface *s= NULL;
	for (Surface *surface : surfaces) {
		float alpha1, alpha2;
		alpha1 = surface->hit(ray, alpha2);
		if (alpha1>0.0 && alpha1 < out_alpha1) {
			out_alpha1 = alpha1;
			out_alpha2 = alpha2;
			s = surface;
		}
	}

	out_alpha = out_alpha1<0.001? out_alpha2: out_alpha1;
	return s;
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
		params = parse_input(filename, surfaces, lights, textures);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	Image img(params.width, params.height, params.bkg_color);

	/* Create the viewing window */
	ViewWindow vw(params);

	/* Fill the image with the nearest ray intersection points */
	srand(time(NULL)); // seed for soft shadows
  /* seed for depth of field */
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::default_random_engine(seed);

	for (Ray& ray : vw.all_rays) {
		float alpha;
		Surface *s = trace_ray(ray, alpha);
		if (s != NULL)
			img(ray.r, ray.c) = get_color(ray, alpha, s);
	}

	/* implement depth of field */
	int bundle_size=1; // set to 1 for no depth of field
	int d = vw.d, count = 1;
	Vec3 eye = params.eye;
	for (; count<=bundle_size; count++) {
		Vec3 jitter(RANDN(), RANDN(), RANDN());
		params.eye = eye + (.009*d*jitter);
		vw = ViewWindow(params);

		for (Ray& ray : vw.all_rays) {
			float alpha;
			Surface *s = trace_ray(ray, alpha);
			if (s != NULL)
				img(ray.r, ray.c) = get_color(ray, alpha, s);
		}

	}
	for (Color& c : img.image)
		c = c/(float) count;


	/* save the image */
	std::string fn = remove_ext(filename) + ".ppm";
	img.save(fn);

	/* clean-up */
	for (LightSource *source : lights)
		delete source;
	for (Surface *surface : surfaces)
		delete surface;
	for (Texture *texture : textures)
		delete texture;

	return 0;
}