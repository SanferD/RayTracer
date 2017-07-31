#include "lights.hpp"

#include <cstdio>

/***************************/
/****** LIGHT SOURCE *******/
/***************************/

LightSource::LightSource() {}
void LightSource::print() {
	printf("LightSource:= (%.3f, %.3f, %.3f) Color:= (%.3f, %.3f, %.3f)\n", p.x, p.y, p.z, c.r, c.g, c.b);
}

/***************************/
/********** LIGHT **********/
/***************************/

Light::Light() {}

Light::Light(float x, float y, float z, float _w, Color c) {
	if (_w == 0.0) {
		Vec3 v(x, y, z);
		v.normalize();
		x = v.x; y = v.y; z = v.z;
	}
	p.x = x;
	p.y = y;
	p.z = z;
	w = _w;
	this->c = c;
}

void Light::print() {
	printf("LIGHT: (%.3f, %.3f, %.3f, %.3f) with color: (%.3f, %.3f, %.3f)\n", 
		p.x, p.y, p.z, w, c.r, c.g, c.b);
}

/***************************/
/****** SPOTLIGHT **********/
/***************************/

Spotlight::Spotlight() {}
Spotlight::Spotlight(float x, float y, float z, float dir_x, float dir_y, float dir_z, float theta, Color c) {
	p.x = x;	p.y = y;	p.z = z;
	dir = Vec3(dir_x, dir_y, dir_z);
	dir.normalize();
	this->theta = theta;
	this->c = c;
	w = -1.0; // unique spotlight identifier
}

void Spotlight::print() {
	printf("SPOTLIGHT: (%.3f, %.3f, %.3f) DIR: (%.3f, %.3f, %.3f) ANGLE: %.3f with color: (%.3f, %.3f, %.3f)\n", 
		p.x, p.y, p.z, dir.x, dir.y, dir.z, theta, c.r, c.g, c.b);
}