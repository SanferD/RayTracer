#ifndef _PARAMS_H
#define _PARAMS_H

#include <cstdio>

#include "geometry.hpp"
#include "image.hpp"
#include "surfaces.hpp"

/**
 * A structure to hold the parameters extracted from the input file.
 */
struct Params {
	Vec3 eye;
	Vec3 view_dir;
	Vec3 up_dir;
	Angle fovv;
	int width, height;
	Color bkg_color;

	void print() {
		printf("eye: %.3f %.3f %.3f\n", eye.x, eye.y, eye.z);
		printf("view_dir: %.3f %.3f %.3f\n", view_dir.x, view_dir.y, view_dir.z);
		printf("up_dir: %.3f %.3f %.3f\n", up_dir.x, up_dir.y, up_dir.z);
		printf("width: %i height: %i\n", width, height);
		printf("bkg_color: %.3f %.3f %.3f\n", bkg_color.r, bkg_color.g, bkg_color.b);
	}
};

#endif