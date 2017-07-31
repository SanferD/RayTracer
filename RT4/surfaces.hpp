#ifndef _SURFACES_HPP
#define _SURFACES_HPP

#include <vector>

#include "geometry.hpp"
#include "image.hpp"

/**
 * Surface is a base class which is inherited by all other surface classes.
 * center is the center of the surface
 * mtl_color is the color of the surface.
 * hit finds the intersection
 * print prints the surface 
 */

class Surface {
public:
	static std::vector<Vec3> vertex_array;
	static std::vector<float> u_array;
	static std::vector<float> v_array;
	static std::vector<Vec3> n_array;
	int t_idx;
	MtlColor mtl_color;
	int type;
	virtual float hit(Ray r, float& out_t_max);
	virtual Vec3 get_normal(Vec3 intersect);
	virtual void print();
	virtual float get_u(Vec3& p);
	virtual float get_v(Vec3& p);
};

/**
 * Sphere is derrived from a surface with radius r.
 */
class Sphere : public Surface {
public:
	Vec3 center;
	float r;
	Sphere();
	Sphere(float x, float y, float z, float r, MtlColor mtl_color);
	void print();
	float hit(Ray r, float& out_t_max);
	Vec3 get_normal(Vec3 intersect);
	float get_u(Vec3& p);
	float get_v(Vec3& p);
};

/**
 * Ellipsoid is derrived from a surface with axis (a, b, c).
 */
class Ellipsoid : public Surface {
public:
	Vec3 center;
	float a, b, c;
	Ellipsoid();
	Ellipsoid(float x, float y, float z, float a, float b, float c, MtlColor mtl_color);
	void print();
	float hit(Ray r, float& out_t_max);
	Vec3 get_normal(Vec3 intersect);
	float get_u(Vec3& p);
	float get_v(Vec3& p);
};

/**
 * Triangle is derived from a surface. Its vertices are v1, v2, v3.
 */
class Triangle : public Surface {
	Vec3 p0, p1, p2;
	float u0, u1, u2, v0, v1, v2;
	Vec3 n0, n1, n2;
	bool has_texture, has_normal;
public:
	float A, B, C, D; // coefficients of equation of plane
	float alpha, beta, gamma;
	Triangle();
	Triangle(int c[3], int n[3], int v[3], MtlColor mtlcolor, int t_idx=-1);
	void print();
	float hit(Ray r, float& out_t_max);
	Vec3 get_normal(Vec3 intersect);
	float get_u(Vec3& p);
	float get_v(Vec3& p);
};

#endif