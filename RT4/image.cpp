#include "image.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define PI 3.14159265

/**********************************/
/************** Image *************/
/**********************************/

/** empty declaration  */
Image::Image() {}

/**
 * @constructor
 * @param width 	the width of the image
 * @param height 	the height of the image
 * @param bkg 		the background color of the image
 */
Image::Image(int width, int height, Color bkg) {
	this->width = width; this->height = height;
	this->aspect = (float)width/(float)height;
	image.reserve(width*height);
	for (int i=0; i!=width*height; i++)
		image[i] = bkg;
}

/**
 * Save the image to an output file with name 'name'
 * @param name The name of the output file
 */
void Image::save(std::string name) {

	#define PIX_PER_LINE 5

	FILE *fp = fopen(name.c_str(), "w");

	/* Header containers */
	fprintf(fp, "P3\n");
	fprintf(fp, "%i %i\n", width, height);
	fprintf(fp, "%i", 255);

	/* Body */
	int how_many_pixels = width*height, count;
	for (count=0; count!=how_many_pixels; count++) {
		/* add a new line when PIX_PER_LINE pixels have been written */
		if (count%PIX_PER_LINE == 0)
			fprintf(fp, "\n");

		/* write the rgb values */
		Color c = image[count];
		int r = round(255*c.r), g = round(255*c.g), b = round(255*c.b);
		fprintf(fp, "%i %i %i ", r, g, b);
	}

	/* cleanup */
	fclose(fp);

}

/**
 * Access the image pixel located at row r and column c 
 * @param  r the row of the image pixel
 * @param  c the column of the image pixel
 * @return   the Pixel found at (c, r)
 */
Pixel& Image::operator()(int r, int c) {
	return image[r*width + c];
}

/***************************************/
/************** ViewWindow *************/
/***************************************/

/** Empty constructor for declaring variables */
ViewWindow::ViewWindow(){}

/** 
 * @constructor
 * @param params 	The parameter structure
 */
ViewWindow::ViewWindow(Params params) {
	bool par = params.parallel;

	view_origin = params.eye;
	view_dir = params.view_dir;
	up_dir = params.up_dir;
	fovv = params.fovv;
	aspect = (float)params.width/(float)params.height;

	u = view_dir.cross(up_dir).normalize();
	v = u.cross(view_dir).normalize();
	d = 1.0;

	height = 2*d*tan((fovv * PI) / (2 * 180.0));
	width = height*aspect;

	if (par)
		d = 0.0;

	ul = view_origin + d*view_dir + (height/2.0)*v - (width/2.0)*u;
	ur = view_origin + d*view_dir + (height/2.0)*v + (width/2.0)*u;
	ll = view_origin + d*view_dir - (height/2.0)*v - (width/2.0)*u;
	lr = view_origin + d*view_dir - (height/2.0)*v + (width/2.0)*u;

	if (par)
		d = 1.0;

	dh = (ur - ul) / (params.width-1);
	dv = (ll - ul) / (params.height-1);

	all_rays.reserve(params.width*params.height);
	for (float r=0; r!=params.height; r++)
		for (float c=0; c!=params.width; c++) {
			Vec3 corner = ul + c*dh + r*dv;
			Ray ray(par? corner: view_origin,  par? view_dir: corner,  par);	
			ray.r = r; ray.c = c;
			// ray.org.print();
			all_rays.push_back(ray);
		}
}

/**
 * Print the properties of the ViewWindow
 */
void ViewWindow::print() {
	printf("**ViewWindow**\n");
	printf("h: %.3f w: %.3f\n", height, width);

	printf("view_origin: "); view_origin.print();
	printf("view_dir: "); view_dir.print();

	printf("u: "); u.print();
	printf("v: "); v.print();

	printf("ul: "); ul.print();
	printf("ur: "); ur.print();
	printf("ll: "); ll.print();
	printf("lr: "); lr.print();

	printf("dh: "); dh.print();
	printf("dv: "); dv.print();

	printf("# of rays: %zu\n", all_rays.size());
}


/*******************/
/******TEXTURE******/
/*******************/

/** Empty constructor for declarations */
Texture::Texture() {}

Texture::Texture(std::string file_name) {
	/* Verify that file exists */
	std::ifstream in( file_name.c_str() );
	if (!in.is_open()) {
		std::cout << "could not open texture file \'" << file_name << "\'\n";
		exit(1);
	}
	
	/* Read entire contents of file into stringstream */
	std::stringstream ss;
	ss << in.rdbuf();
	in.close();

	/* Get the width and height from the header */
	int width, height; std::string d;
	ss >> d >> width >> height >> d;
	img = Image(width, height);
	
	/* read r g b values */
	for(int index=0; index < width*height; index++) {
		int r, g, b;
		ss >> r >> g >> b;
		img(index/width, index%width) = Pixel((float)r/255.0, (float)g/255.0, (float)b/255.0);
	}

}

void Texture::print() {
	std::cout << "width: " << img.width << " height: " << img.height << std::endl;
}

/**
 * Get the texture color with row and column inputs in [0,1.0] range
 * @param  r Row
 * @param  c Column
 * @return   Pixel at the location
 */
Pixel Texture::operator()(float u, float v) {
	u = u>1.0? 1.0: (u<0.0? 0.0: u);
	v = v>1.0? 1.0: (v<0.0? 0.0: v);
	float x = u*(img.width-1);
	float y = v*(img.height-1);

	int i = (int) x;
	int j = (int) y;

	float alpha = x - (float) i;
	float beta = y - (float) j;

	Color ret = (1.0-alpha)*(1.0-beta)*img(j, i);
	if (i+1<img.width)
		ret = ret + alpha*(1.0-beta)*img(j, i+1);
	if (j+1<img.height)
		ret = ret + (1.0-alpha)*beta*img(j+1, i);
	if (i+1<img.width && j+1<img.height)
		ret = ret + alpha*beta*img(j+1, i+1);
	return ret;
}
