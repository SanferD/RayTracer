#include "utils.hpp"

#include <cstdlib>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>

#include "geometry.hpp"
#include "image.hpp"
#include "params.hpp"
#include "allexceptions.hpp"

#define VALID(c) (c>=0.0&&c<=1.0)

/**
 * Reads the input from the file given by filename and gets the parameters and
 * surfaces. 
 * @param  filename the name of the input file
 * @param  surfaces output parameter which is a vector of Surface pointers.
 * @return          the parameter object is returned
 */
Params parse_input(std::string filename, std::vector<Surface*>& surfaces) {
	std::ifstream in( filename.c_str() );
	Params params;

	if (!in.is_open()) {
		std::cout << "could not open file \'" << filename << "\'\n";
		exit(1);
	}

	bool gotit[6] = {false};

	std::string line = "";
	Color mtlcolor;
	while (std::getline(in, line)) {
		std::stringstream ss(line);
		std::string keyword = "";
		ss >> keyword;
		if (keyword == "")
			continue;

		if (keyword == "eye") {
			float x, y, z; z=NAN;
			ss >> x >> y >> z;
			params.eye = Vec3(x, y, z);

			if (isnan(z))
				throw invalid_scene_file();
			gotit[0] = true;
		}
		else if (keyword == "viewdir") {
			float x, y, z; z=NAN;
			ss >> x >> y >> z;
			params.view_dir = Vec3(x, y, z);

			if (isnan(z))
				throw invalid_scene_file();
			if (fabs(params.view_dir.norm()-1.0) > 0.05)
				throw invalid_vector(params.view_dir, "viewdir");
			
			gotit[1] = true;
		}
		else if (keyword == "updir") {
			float x, y, z; z=NAN;
			ss >> x >> y >> z;
			params.up_dir = Vec3(x, y, z);
			
			if (isnan(z))
				throw invalid_scene_file();
			if (fabs(params.up_dir.norm()-1.0) > 0.05)
				throw invalid_vector(params.up_dir, "updir");
		
			gotit[2] = true;
		}
		else if (keyword == "fovv") {
			params.fovv = NAN;
			ss >> params.fovv;

			if (isnan(params.fovv))
				throw invalid_scene_file();
			if (params.fovv >= 180)
				throw invalid_fov();
		
			gotit[3] = true;
		}
		else if (keyword == "imsize") {
			ss >> params.width >> params.height;
			if (params.width<=0 || params.height<=0)
				throw invalid_dims();
		
			gotit[4] = true;
		}
		else if (keyword == "bkgcolor") {
			float r, g, b; b=NAN;
			ss >> r >> g >> b;
			params.bkg_color = Color(r, g, b);

			if (isnan(b))
				throw invalid_scene_file();
			if (!VALID(r) || !VALID(g) || !VALID(b))
				throw invalid_color();
		
			gotit[5] = true;
		}
		else if (keyword == "mtlcolor") {
			float r, g, b; b=NAN;
			ss >> r >> g >> b;
			mtlcolor = Color(r, g, b);

			if (isnan(b))
				throw invalid_scene_file();
			if (!VALID(r) || !VALID(g) || !VALID(b))
				throw invalid_color();
		}
		else if (keyword == "sphere") {
			float x, y, z, r; r=NAN;
			ss >> x >> y >> z >> r;

			if (isnan(r) || r<=0.0)
				throw invalid_scene_file();

			Surface *s = new Sphere(x, y, z, r, mtlcolor);
			surfaces.push_back(s);
		}
		else if (keyword == "ellipsoid") {
			float x, y, z, rx, ry, rz; rz=NAN;
			ss >> x >> y >> z >> rx >> ry >> rz;

			if (isnan(rz) || rx<=0.0 || ry<=0.0 || rz<=0.0)
				throw invalid_scene_file();

			Surface *e = new Ellipsoid(x, y, z, rx, ry, rz, mtlcolor);
			surfaces.push_back(e);
		}
		else 
			throw invalid_scene_file();
	}

	// check if we read all the necessary parameters
	for (int i=0; i!=6; i++)
		if (!gotit[i])
			throw invalid_scene_file();
	
	// check if updir and viewdir are not parallel
	float eps = 0.05;
	if (params.up_dir.cross(params.view_dir).norm() < eps)
		throw parallel_coords();


	return params;
}
