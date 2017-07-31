#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#include "geometry.hpp"
#include "image.hpp"
#include "params.hpp"
#include "utils.hpp"
#include "surfaces.hpp"

int main(int argc, char *argv[]) {
	/* Basic input validation */
	if (argc != 2) {
		printf("usage: ./main <input-file>\n");
		exit(1);
	}

	std::vector<Surface*> surfaces;
	std::string filename(argv[1]);

	/* Get image parameters and initialize the image */
	Params params;
	try {
		params = parse_input(filename, surfaces);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	Image img(params.width, params.height, params.bkg_color);

	/* Create the viewing window */
	ViewWindow vw(params);

	/* Fill the image with the nearest ray intersection points */
	for (auto iter1=vw.all_rays.begin(); iter1!=vw.all_rays.end(); iter1++) {
		Ray ray = *iter1;
		Color mtl_color;
		float alpha_max = INFINITY; 
		
		for (auto iter2=surfaces.begin(); iter2!=surfaces.end(); iter2++) {
			Surface *surface = *iter2;
			float alpha = surface->hit(ray);
			if (alpha != -1.0 && alpha < alpha_max) {
				alpha_max = alpha;
				mtl_color = surface->mtl_color;
			}
		}

		if (alpha_max != INFINITY)
			img(ray.r, ray.c) = mtl_color;
	}

	/* save the image */
	img.save();

	/* clean-up */
	for (auto iter=surfaces.begin(); iter!=surfaces.end(); iter++)
		delete *iter;

	return 0;
}