#ifndef _SURFACES_HPP
#define _SURFACES_HPP

#include "geometry.hpp"

/**
 * Surface is a base class which is inherited by all other surface classes.
 * center is the center of the surface
 * mtl_colot is the color of the surface.
 * hit finds the intersection
 * print prints the surface 
 */

class Surface {
public:
	Vec3 center;
	Color mtl_color;
	virtual float hit(Ray r);
	virtual void print();
};

/**
 * Sphere is derrived from a surface with radius r.
 */
class Sphere : public Surface {
public:
	float r;
	Sphere();
	Sphere(float x, float y, float z, float r, Color mtl_color);
	void print();
	float hit(Ray r);
};

/**
 * Ellipsoid is derrived from a surface with axis (a, b, c).
 */
class Ellipsoid : public Surface {
public:
	float a, b, c;
	Ellipsoid();
	Ellipsoid(float x, float y, float z, float a, float b, float c, Color mtl_color);
	void print();
	float hit(Ray r);
};

#endif