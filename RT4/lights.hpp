#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include "image.hpp"

class LightSource {
public:
	Vec3 p;
	float w;
	Color c;
	LightSource();
	virtual void print();
};

/**
 * Light
 */
class Light : public LightSource {
public:
	Light();
	Light(float x, float y, float z, float w, Color c);
	void print();
};

/**
 * Spotlight
 */
class Spotlight : public LightSource {
public:
	Vec3 dir; 
	float theta;
	Spotlight();
	Spotlight(float x, float y, float z, float dir_x, float dir_y, float dir_z, float theta, Color c);
	void print();
};

#endif