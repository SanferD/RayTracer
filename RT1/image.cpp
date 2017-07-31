#include "image.hpp"

#include <cmath>
#include <cstdio>
#include <string>

#define PI 3.14159265

/**********************************/
/************** Image *************/
/**********************************/

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

	ul = view_origin + d*view_dir + (height/2.0)*v - (width/2.0)*u;
	ur = view_origin + d*view_dir + (height/2.0)*v + (width/2.0)*u;
	ll = view_origin + d*view_dir - (height/2.0)*v - (width/2.0)*u;
	lr = view_origin + d*view_dir - (height/2.0)*v + (width/2.0)*u;

	dh = (ur - ul) / (params.width-1);
	dv = (ll - ul) / (params.height-1);

	all_rays.reserve(params.width*params.height);
	for (float r=0; r!=params.height; r++)
		for (float c=0; c!=params.width; c++) {
			Vec3 corner = ul + c*dh + r*dv;
			Ray ray(view_origin, corner);	ray.r = r; ray.c = c;
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