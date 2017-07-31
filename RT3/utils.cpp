#include "utils.hpp"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>

#include "allexceptions.hpp"
#include "geometry.hpp"
#include "image.hpp"
#include "lights.hpp"
#include "params.hpp"

#define VALID(c) (c>=0.0&&c<=1.0)

/**
 * Reads the input from the file given by filename and gets the parameters and
 * surfaces. 
 * @param  filename the name of the input file
 * @param  surfaces output parameter which is a vector of Surface pointers.
 * @param  lights output parameter which is a vector of LightSource pointers.
 * @return          the parameter object is returned
 */
Params parse_input(std::string filename, std::vector<Surface*>& surfaces, std::vector<LightSource*>& lights, std::vector<Texture*>& textures) {
	std::ifstream in( filename.c_str() );
	Params params;
	params.parallel = false;
	std::string path = get_path(filename);
	if (!in.is_open()) {
		std::cout << "could not open file \'" << filename << "\'\n";
		exit(1);
	}

	bool gotit[6] = {false};

	std::string line = "";
	MtlColor mtlcolor;
	int t_idx=-1;
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

			if (std::isnan(z))
				throw invalid_scene_file();
			gotit[0] = true;
		}
		else if (keyword == "viewdir") {
			float x, y, z; z=NAN;
			ss >> x >> y >> z;
			params.view_dir = Vec3(x, y, z);

			if (std::isnan(z))
				throw invalid_scene_file();
			if (fabs(params.view_dir.norm()-1.0) > 0.05)
				params.view_dir.normalize();
			
			gotit[1] = true;
		}
		else if (keyword == "updir") {
			float x, y, z; z=NAN;
			ss >> x >> y >> z;
			params.up_dir = Vec3(x, y, z);
			
			if (std::isnan(z))
				throw invalid_scene_file();
			if (fabs(params.up_dir.norm()-1.0) > 0.05)
				params.up_dir.normalize();
		
			gotit[2] = true;
		}
		else if (keyword == "fovv") {
			params.fovv = NAN;
			ss >> params.fovv;

			if (std::isnan(params.fovv))
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

			if (std::isnan(b))
				throw invalid_scene_file();
			if (!VALID(r) || !VALID(g) || !VALID(b))
				throw invalid_color();
		
			gotit[5] = true;
		}
		else if (keyword == "mtlcolor") {
			float r1, g1, b1, r2, g2, b2, ka, kd, ks, n=NAN;
			ss >> r1 >> g1 >> b1 >> r2 >> g2 >> b2 >> ka >> kd >> ks >> n;
			mtlcolor = MtlColor(Color(r1, g1, b1), Color(r2, g2, b2), ka, kd, ks, n);

			if (std::isnan(n))
				throw invalid_scene_file();
			if (!VALID(r1) || !VALID(g1) || !VALID(b1) || 
					!VALID(r2) || !VALID(g2) || !VALID(b2) )
				throw invalid_color();
			if (!VALID(ka) || !VALID(kd) || !VALID(ks))
				throw invalid_constant();
		}
		else if (keyword == "sphere") {
			float x, y, z, r; r=NAN;
			ss >> x >> y >> z >> r;

			if (std::isnan(r) || r<=0.0)
				throw invalid_scene_file();

			Surface *s = new Sphere(x, y, z, r, mtlcolor);
			s->t_idx = t_idx;
			surfaces.push_back(s);
		}
		else if (keyword == "ellipsoid") {
			float x, y, z, rx, ry, rz; rz=NAN;
			ss >> x >> y >> z >> rx >> ry >> rz;

			if (std::isnan(rz) || rx<=0.0 || ry<=0.0 || rz<=0.0)
				throw invalid_scene_file();

			Surface *e = new Ellipsoid(x, y, z, rx, ry, rz, mtlcolor);
			e->t_idx = t_idx;
			surfaces.push_back(e);
		}
		else if (keyword == "f") {
			int n[3]={0}, c[3]={0}, v[3], beg, end;
			for (int i=0; i!=3; i++) {
				std::string token;
				ss >> token;
				if (token.length() == 0)
					throw invalid_scene_file();
				
				beg = token.find("/");
				end = token.rfind("/");
				if ( beg==std::string::npos ) { // no forward slash
					v[i] = std::atoi( token.c_str() );
				}
				else if (beg == end) { // only one forward slash
					v[i] = std::atoi( token.substr(0, beg).c_str() );
					c[i] = std::atoi( token.substr(beg+1).c_str() );
				}
				else { // two forward slashes
					v[i] = std::atoi( token.substr(0, beg).c_str() );
					c[i] = std::atoi( token.substr(beg+1, end-beg-1).c_str() );
					n[i] = std::atoi( token.substr(end+1).c_str() );
				}
				if (c[i] > Surface::u_array.size() || n[i] > Surface::n_array.size())
					throw invalid_scene_file();
			}
			Triangle *t = new Triangle(c, n, v, mtlcolor, t_idx);
			surfaces.push_back(t);
		}
		else if (keyword == "v") {
			float x, y, z; z=NAN;
			ss >> x >> y >> z;
			if (std::isnan(z))
				throw invalid_scene_file();
			Surface::vertex_array.push_back(Vec3(x, y, z));
		}
		else if (keyword == "vt") {
			float u, v; v=NAN;
			ss >> u >> v;
			if (std::isnan(v))
				throw invalid_scene_file();
			if (!VALID(u) || !VALID(v))
				throw invalid_scene_file();
			Surface::u_array.push_back(u);
			Surface::v_array.push_back(v);
		}
		else if (keyword == "vn"){
			float nx, ny, nz; nz = NAN;
			ss >> nx >> ny >> nz;
			if (std::isnan(nz))
				throw invalid_scene_file();
			Surface::n_array.push_back(Vec3(nx, ny, nz));
		}
		else if (keyword == "light") {
			float x, y, z, w, r, g, b; b=NAN;
			ss >> x >> y >> z >> w >> r >> g >> b;

			if (std::isnan(b))
				throw invalid_scene_file();
			if (!VALID(r) || !VALID(g) || !VALID(b))
				throw invalid_color();

			LightSource *l = new Light(x, y, z, w, Color(r, g, b));
			lights.push_back(l);
		}
		else if (keyword == "spotlight") {
			float x, y, z, dir_x, dir_y, dir_z, theta, r, g, b; b=NAN;
			ss >> x >> y >> z >> dir_x >> dir_y >> dir_z >> theta >> r >> g >> b;

			if (std::isnan(b))
				throw invalid_scene_file();
			if (!VALID(r) || !VALID(g) || !VALID(b))
				throw invalid_color();
			
			LightSource *l = new Spotlight(x, y, z, dir_x, dir_y, dir_z, theta, Color(r, g, b));
			lights.push_back(l);
		}
		else if (keyword == "texture") {
			std::string fn;
			ss >> fn;
			textures.push_back(new Texture(path+fn));
			t_idx++;
		}
		else if (keyword == "parallel")
			params.parallel = true;
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

/* Removes the extension of mystr. The input string is unaltered.
 * The returned string is a copy of the original without extension.
 * Assumes that extension does not cotain any slash. 
 */
std::string remove_ext(std::string str) {
	char *mystr = (char *) str.c_str();
  char *s = strrchr(mystr, '.');
  if (!s)
    return str; 

  char *b1 = strrchr(s, '\\');
  char *b2 = strrchr(s, '/');

  if (!b1 && !b2) {
    // remove any consecutive .
    while((s-mystr) && *(s-1)=='.') --s;

    int length = s-mystr;
    char *tmp = (char*) malloc( (length+1)*sizeof(char) );
    if (tmp == NULL) {
    	printf("Malloc error\n");
    	exit(1);
    }
    strncpy(tmp, mystr, length);
    tmp[length] = '\0';
    return std::string(tmp);
  }

  return std::string(mystr);
}

/**
 * Gets the relative path. So if input is main.cpp, it will return "./"
 * If the input is "~/main.cpp" it will return "~/". 
 * Assumes '/' is used to separate paths.
 * @param  full_path the full path
 * @return           the directory
 */
std::string get_path(std::string full_path) {
	int end = full_path.rfind("/");
	
	if (end == std::string::npos)
		return std::string("./");
	else
		return full_path.substr(0, end+1);
}