#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include <vector>

/**
 * Color object which is a container for r,g,b values.
 */
class Color {
public:
	float r, g, b;
	Color(float r, float g, float b);
	Color();
};

/**
 * Vec3 object for a 3D vector. Points are also Vec3s.
 * Operations are:
 * 	norm:	length of the vector
 * 	normalize:	make the vector a unit vector
 * 	dot:	dot this vector with another one
 * 	cross:	cross this vector with another one
 * 	print:	print the coordinates of the vector 
 */
class Vec3 {
public:
	float x, y, z;
	Vec3();
	Vec3(float x, float y, float z);
	float norm();
	Vec3& normalize();
	float dot(Vec3 other_v);
	Vec3 cross(Vec3 other_v);
	void print();

	friend Vec3 operator+(Vec3 one, Vec3 two);
	friend Vec3 operator-(Vec3 one, Vec3 two);
	friend Vec3 operator*(float c, Vec3 p);
	friend Vec3 operator/(Vec3 vec, float f);

};

/** Angle is just a float */
typedef float Angle;


/** Ray is a functor with origin org and directional vector dir.
 * To evaluate the ray for some t in ray(t) = org + t*dir, 
 * use the operator() with float t as argument. 
 */
class Ray {
public:
	Vec3& org; Vec3 dir;
	int r, c;
	Ray(Vec3& e, Vec3 s) : org(e), dir((s-e).normalize()) {}
	Vec3 operator()(float t) {
		return org + t*dir;
	} 
};

#endif