#ifndef _IMAGE_H
#define _IMAGE_H

#include <string>
#include <vector>

#include "geometry.hpp"
#include "params.hpp"

/** For readability, Pixel is a Color. */
typedef Color Pixel;

/** 
 * Image object which represents the image. Use the operator(r, c) 
 * to access the pixel found at row r and column c, 0 indexed. 
 */
class Image {
	float aspect;

public:
	std::vector<Pixel> image; 
	int width, height;
	Image();
	Image(int width, int height, Color bkg = Color(0.0, 0.0, 0.0));
	Pixel& operator()(int r, int c);
	void save(std::string name="output.ppm");
};

/**
 * Viewing Window. End goal is to get all the rays going from the origin
 * through a pixel in the viewing window.
 */
class ViewWindow {
	Vec3 ul, ur, ll, lr;
	Vec3 view_origin;
	Vec3 u, v, view_dir, up_dir;
	Angle fovv;
	float height, width;
	Vec3 dh, dv;
public:
	float aspect, d;
	std::vector<Ray> all_rays;
	ViewWindow();
	ViewWindow(struct Params);
	void print();
};

/**
 * Texture object.
 */
class Texture {
	Image img;
public:
	Texture();
	Texture(std::string file_name);
	Pixel operator()(float u, float v);
	void print();
};

#endif