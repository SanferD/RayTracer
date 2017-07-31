#include "surfaces.hpp"

#include <cmath>
#include <cstdio>

/**
 * Given A, B, C, it solves the quadratic equation and returns the 
 * smallest solution, or -1 if no solution exists. 
 * @param  A t^2 term
 * @param  B t term
 * @param  C constant
 * @return the smallest solution or -1 if not solution was found
 */
float solve_quadratic(float A, float B, float C) {
	float disc = pow(B, 2.0) - 4*A*C;
	if (disc < 0.0)
		return -NAN;

	float t1, t2;
	t1 = (-B + sqrt(disc))/(2*A);
	t2 = (-B - sqrt(disc))/(2*A);

	return t1<t2? t1: t2;
}

/**************************************/
/************** Surface ***************/
/**************************************/

/** Virtual function for hit. Not supposed to be used.  */
float Surface::hit(Ray r) {
	return -1.0;
}

/** Virtual function for print. Not supposed to be used. */
void Surface::print() {
	printf("I\'m a surface\n");
}

/*************************************/
/************** Sphere ***************/
/*************************************/

/** Empty constructor for declaration variables.  */
Sphere::Sphere() {}

/**
 * @constructor
 * Constructor for Sphere which is derived from Surface.
 * @param x		the x coordinate of the center
 * @param y		the y coordinate of the center
 * @param z		the z coordinate of the center
 * @param r 	the radius of the sphere
 * @param mtl_color	the color of the material
 */
Sphere::Sphere(float x, float y, float z, float r, MtlColor mtl_color) {
	center = Vec3(x, y, z);
	this->r = r;
	this->mtl_color = mtl_color;
}

/**
 * Prints the sphere's information
 */
void Sphere::print() {
	printf("Sphere:= Center: (%.3f, %.3f, %.3f) R: %.3f ", center.x, center.y, center.z, r);
	mtl_color.print();
}

/**
 * Checks if the given ray intersects the sphere.
 * Otherwise -1.0 is returned.
 * @param  r The Ray
 * @return -1.0 if no intersection. Otherwise the smallest value of the param t
 *              in r(t) such that r(t) lies lies on the sphere. 
 */
float Sphere::hit(Ray r) {
	Vec3 dif = r.org-center;

	float A = 1.0;
	float B = 2.0*( r.dir.dot(dif) );
	float C = dif.dot(dif) - pow(this->r, 2.0);
	return solve_quadratic(A, B, C);
}


/*************************************/
/************* Ellipsoid *************/
/*************************************/

/** Empty constructor for declaration variables.  */
Ellipsoid::Ellipsoid() {}

/**
 * @constructor
 * Constructor for Ellipsoid which is derived from Surface.
 * @param x		the x coordinate of the center
 * @param y		the y coordinate of the center
 * @param z		the z coordinate of the center
 * @param a 	the first axis of the ellipsoid
 * @param b 	the second axis of the ellipsoid
 * @param c 	the third axis of the ellipsoid
 * @param mtl_color	the color of the material
 */
Ellipsoid::Ellipsoid(float x, float y, float z, float a, float b, float c, MtlColor mtl_color) {
	center = Vec3(x, y, z);
	this->a = a; this->b = b; this->c = c;
	this->mtl_color = mtl_color;
}

/**
 * Prints the information of the Ellipsoid
 */
void Ellipsoid::print() {
	printf("Ellipsoid:= Center: (%.3f, %.3f, %.3f) Axis: (%.3f, %.3f, %.3f) ", center.x, center.y, center.z, a, b, c);
	mtl_color.print();
}

/**
 * Checks if the Ray r intersects the Ellipsoid.
 * @param  r The ray
 * @return   -1.0 is no intersection. Otherwise the smallest value of t in
 *            r(t) such that r(t) lies on the ellipsoid.
 */
float Ellipsoid::hit(Ray r) {
	float A = pow(r.dir.x, 2.0)/pow(a, 2.0) + 
						pow(r.dir.y, 2.0)/pow(b, 2.0) +
						pow(r.dir.z, 2.0)/pow(c, 2.0);

	float B = ((r.org.x - center.x)*r.dir.x)/pow(a, 2.0) +
						((r.org.y - center.y)*r.dir.y)/pow(b, 2.0) +
						((r.org.z - center.z)*r.dir.z)/pow(c, 2.0);
	B *= 2.0;
	float C = 	pow(r.org.x-center.x, 2.0)/pow(a, 2.0) +
				pow(r.org.y-center.y, 2.0)/pow(b, 2.0) +
				pow(r.org.z-center.z, 2.0)/pow(c, 2.0) -
				1.0;

	return solve_quadratic(A, B, C);
}