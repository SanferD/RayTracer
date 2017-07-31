#include <cmath>
#include <cstdio>

#include "geometry.hpp"

/************************************/
/************* Color ****************/
/************************************/

/** Empty constructor for declaring variables */
Color::Color() {}


/**
 * @constructor
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 */
Color::Color(float r, float g, float b) {
	this->r = r; this->g = g; this->b = b;
}

Color operator+(Color one, Color two) {
	return Color(one.r+two.r, one.g+two.g, one.b+two.b);
}

Color operator-(Color one, Color two) {
	return Color(one.r-two.r, one.g-two.g, one.b-two.g);
}

Color operator*(Color one, Color two) {
	return Color(one.r*two.r, one.g*two.g, one.b*two.b);
}

Color operator*(float k, Color c) {
	return Color(k*c.r, k*c.g, k*c.b);
}

Color operator*(Color c, float k) {
	return Color(k*c.r, k*c.g, k*c.b);
}

Color operator/(Color c, float k) {
	return Color(c.r/k, c.g/k, c.b/k);
}


/************************************/
/*********** MtlColor ***************/
/************************************/

/**
 * @constructor
 */
MtlColor::MtlColor(Color Od, Color Os, float ka, float kd, float ks, float n) {
	this->Od = Od;
	this->Os = Os;
	this->ka = ka;
	this->kd = kd;
	this->ks = ks;
	this->n = n;
}

/** Empry constructor */
MtlColor::MtlColor() {}

void MtlColor::print() {
	printf("MTLCOLOR: Od: (%.3f, %.3f, %.3f), Os: (%.3f, %.3f, %.3f) ka: %.3f kd: %.3f ks: %.3f n: %.3f\n",
		Od.r, Od.g, Od.b, Os.r, Os.g, Os.b, ka, kd, ks, n);
}


/***********************************/
/************* Vec3 ****************/
/***********************************/

/** Empty constructor */
Vec3::Vec3() {}

/**
 * @constructor
 * @param x 	the x component of the vector
 * @param y 	the y component of the vector
 * @param z 	the z component of the vector
 */
Vec3::Vec3(float x, float y, float z) {
	this->x = x; this->y = y; this->z = z;
}

/**
 * Get the length of the vector
 * @return the length of the vector
 */
float Vec3::norm() {
	return sqrt(x*x + y*y + z*z);
}

/**
 * Normalize the vector. It changes the current vector.
 * @return Returns this vector for chaining.
 */
Vec3& Vec3::normalize() {
	float length = this->norm();
	x /= length;
	y /= length;
	z /= length;
	return *this;
}

/**
 * Dot product this_vector * other_vector
 * @param  other_v the RHS of the dot product
 * @return         the dot product of the two vectors
 */
float Vec3::dot(Vec3 other_v) {
	return x*other_v.x + y*other_v.y + z*other_v.z;
}

/**
 * Cross product this_vector * other_vector
 * @param  other_v the RHS of the cross product
 * @return         the cross product vector of the two vectors
 */
Vec3 Vec3::cross(Vec3 other_v) {
	float xp = y*other_v.z - z*other_v.y;
	float yp = z*other_v.x - x*other_v.z;
	float zp = x*other_v.y - y*other_v.x;
	return Vec3(xp, yp, zp);
}

/**
 * Print the vector components
 */
void Vec3::print() {
	printf("(%.3f, %.3f, %.3f)\n", x, y, z);
}

/** Overloaded operators */

Vec3 operator+(Vec3 one, Vec3 two) {
	return Vec3(one.x+two.x, one.y+two.y, one.z+two.z);
}

Vec3 operator-(Vec3 one, Vec3 two) {
	return Vec3(one.x-two.x, one.y-two.y, one.z-two.z);
}

Vec3 operator*(float c, Vec3 p) {
	return Vec3(c*p.x, c*p.y, c*p.z);
}

Vec3 operator*(Vec3 p, float c) {
	return Vec3(c*p.x, c*p.y, c*p.z);
}

Vec3 operator/(Vec3 vec, float f) {
	return Vec3(vec.x/f, vec.y/f, vec.z/f);
}

Vec3 operator*(Vec3 one, Vec3 two) {
	return Vec3(one.x*two.x, one.y*two.y, one.z*two.z);
}

