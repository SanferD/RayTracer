#include "surfaces.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

/**
 * Given A, B, C, it solves the quadratic equation and returns the 
 * smallest solution, or -1 if no solution exists. 
 * @param  A t^2 term
 * @param  B t term
 * @param  C constant
 * @return the smallest solution or -1 if not solution was found
 */
float solve_quadratic(float A, float B, float C, float &out_t_max) {
	float disc = pow(B, 2.0) - 4.0*A*C;
	if (disc < 0.0)
		return -NAN;

	float t1, t2;
	t1 = (-B + sqrt(disc))/(2.0*A);
	t2 = (-B - sqrt(disc))/(2.0*A);

	out_t_max = t1>t2? t1: t2; 
	return t1<t2? t1: t2;
}

/**************************************/
/************** Surface ***************/
/**************************************/

// required for some reason
std::vector<Vec3> Surface::vertex_array;
std::vector<float> Surface::u_array;
std::vector<float> Surface::v_array;
std::vector<Vec3> Surface::n_array;

/** Virtual function for hit. Not supposed to be used.  */
float Surface::hit(Ray r, float& out_t_max) {
	return out_t_max=-1.0;
}

/** Virtual function for print. Not supposed to be used. */
void Surface::print() {
	printf("I\'m a surface\n");
}

/** Virtual function for get_normal. Don't use.  */
Vec3 Surface::get_normal(Vec3 intersect) {
	return intersect;
}

/** Virtual function for get_x. Don't use.  */
float Surface::get_u(Vec3& p) {
	return -1.0;
}

/** Virtual function for get_y. Don't use.  */
float Surface::get_v(Vec3& p) {
	return -1.0;
}


/*************************************/
/************** Sphere ***************/
/*************************************/

/** Empty constructor for declaration variables.  */
Sphere::Sphere() { type=1; }

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
	type=1;
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
float Sphere::hit(Ray r, float& out_t_max) {
	Vec3 dif = r.org-center;

	float A = 1.0;
	float B = 2.0*r.dir.dot(dif);
	float C = dif.dot(dif) - pow(this->r, 2.0);
	return solve_quadratic(A, B, C, out_t_max);
}

/**
 * Returns the surface normal at the point of intersection
 * @param  intersect The point of intersection on the surface
 * @return           The unit normal vector N 
 */
Vec3 Sphere::get_normal(Vec3 intersect) {
	return (intersect - center).normalize();
}

/**
 * Get the x texture coordinate at point p
 * @param  p The point on sphere
 * @return   The x texture coordinate
 */
float Sphere::get_u(Vec3& p) {
	float theta = std::atan2(p.y-center.y, p.x-center.x);
	assert((!std::isnan(theta)));
	if (theta < 0)
		theta += 2*M_PI;
	theta /= 2*M_PI;
	return theta;
}

/**
 * Get the y texture coordinate at point p
 * @param  p The point on sphere
 * @return   The y texture coordinate
 */
float Sphere::get_v(Vec3& p) {
	float ratio = (p.z-center.z)/r;
	float phi = std::acos(ratio>1.0? 1.0: ratio);
	assert((!std::isnan(phi)));
	phi /= M_PI;
	return phi;
}

/*************************************/
/************* Ellipsoid *************/
/*************************************/

/** Empty constructor for declaration variables.  */
Ellipsoid::Ellipsoid() { type=2; }

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
	type = 2;
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
float Ellipsoid::hit(Ray r, float& out_t_max) {
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

	return solve_quadratic(A, B, C, out_t_max);
}

/**
 * Returns the surface normal at the point of intersection
 * @param  intersect The point of intersection on the surface
 * @return           The unit normal vector N 
 */
Vec3 Ellipsoid::get_normal(Vec3 intersect) {
	return (intersect - center).normalize();
}

/**
 * Get the x texture coordinate at point p
 * @param  p The point on sphere
 * @return   The x texture coordinate
 */
float Ellipsoid::get_u(Vec3& p) {
	float theta = std::atan2(a*(p.y-center.y), b*(p.x-center.x));
	if (theta < 2*M_PI)
		theta += 2*M_PI;
	return theta/(2*M_PI);
}

/**
 * Get the y texture coordinate at point p
 * @param  p The point on sphere
 * @return   The y texture coordinate
 */
float Ellipsoid::get_v(Vec3& p) {
	float phi = std::acos((p.z-center.z)/c);
	return phi/M_PI;
}


/***************************/
/*********TRIANGLE**********/
/***************************/

/** Empty constructor for declaration purposes  */
Triangle::Triangle() { type=3; }

/**
 * @constructor
 * Sets up the vertices of the triangle and calculates the plane of the triangle.
 * All input indeces start from 1. Actually decremented inside constructor.
 * @param first 	The index in the vertex array of the first vertex
 * @param second 	The index in the vertex array of the second vertex
 * @param third 	The index in the vertex array of the third vertex
 */
Triangle::Triangle(int c[3], int n[3], int v[3], MtlColor color, int tidx) {
	p0 = vertex_array[v[0]-1];
	p1 = vertex_array[v[1]-1];
	p2 = vertex_array[v[2]-1];
	type = 3;

	if (has_texture=(c[0]!=0)) {
		u0 = u_array[c[0]-1];
		u1 = u_array[c[1]-1];
		u2 = u_array[c[2]-1];

		v0 = v_array[c[0]-1];
		v1 = v_array[c[1]-1];
		v2 = v_array[c[2]-1];
	}

	t_idx = has_texture? tidx: -1;

	if (has_normal=(n[0]!=0)) {
		n0 = n_array[n[0]-1];
		n1 = n_array[n[1]-1];
		n2 = n_array[n[2]-1];
	}

	mtl_color = color;

	Vec3 e1 = p1-p0, e2 = p2-p0, normal = e1.cross(e2).normalize();
	A = normal.x, B = normal.y, C = normal.z, D = -(A*p0.x + B*p0.y + C*p0.z);
}

/**
 * Prints the vertices of the triangle.
 */
void Triangle::print() {
	printf("Triangle:= p0(%.3f, %.3f, %.3f) p1(%.3f, %.3f, %.3f) p2(%.3f, %.3f, %.3f) t_idx:%i\n", p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, t_idx);
}

/**
 * Calculate the triangle intersection
 * @param  ray The ray to test for intersection
 * @return     The t value at the point of intersection, or -1 otherwise.
 */
float Triangle::hit(Ray ray, float& out_t_max) {
	#define _IS_IN(x) (x>=0.0 && x<=1.0)
	#define _TRI_EPS (.0001)

	Vec3 coeff(A, B, C);
	float t = -(coeff.dot(ray.org) + D)/(coeff.dot(ray.dir));
	if (std::isnan(t)) // ray is parallel to the plane
		return -1.0;

	Vec3 p = ray(t), e1 = p1-p0, e2 = p2-p0, e3 = p-p1, e4 = p-p2;
	float area = .5*e1.cross(e2).norm(), a = .5*e3.cross(e4).norm(), b = .5*e4.cross(e2).norm(), c = .5*e1.cross(e3).norm();
	alpha = a/area, beta = b/area, gamma = c/area;
	return out_t_max=(_IS_IN(alpha) && _IS_IN(beta) && _IS_IN(gamma) && alpha+beta+gamma-1.0<=_TRI_EPS)? t: -1.0;
}

/**
 * Return the normal of the triangle at the point of intersection
 * @param  intersect The point of intersection
 * @return           [description]
 */
Vec3 Triangle::get_normal(Vec3 intersect) {
	return has_normal	? (alpha*n0 + beta*n1 + gamma*n2).normalize()
										: Vec3(A, B, C);
}

/**
 * Get the u texture coordinate at point p. Assumes is called after hit is called.
 * @param  p The point on sphere
 * @return   The u texture coordinate
 */
float Triangle::get_u(Vec3& p) {
	if (!has_texture) {
		std::cout << "attempting to access texture coordinate of not textured triangle" << std::endl;
		this->print();
		exit(1);
	}
	return alpha*u0 + beta*u1 + gamma*u2;
}

/**
 * Get the u texture coordinate at point p. Assumes is called after hit is called.
 * @param  p The point on sphere
 * @return   The u texture coordinate
 */
float Triangle::get_v(Vec3& p) {
	if (!has_texture) {
		std::cout << "attempting to access texture coordinate of not textured triangle" << std::endl;
		this->print();
		exit(1);
	}
	return alpha*v0 + beta*v1 + gamma*v2;
}
