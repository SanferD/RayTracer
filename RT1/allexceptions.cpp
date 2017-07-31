#include <cstring>
#include <cstdio>

#include "allexceptions.hpp"

/*****************************************/
/************ invalid_vector *************/
/*****************************************/
invalid_vector::invalid_vector(Vec3 v, std::string vec_name) {
	vec = v;
	this->vec_name = std::string(vec_name);
}

const char* invalid_vector::what() const throw() {
	char msg[256] = "", name[256] = "";
	
	if (vec_name.length() != 0)
		sprintf(name, "\'%s\' =", vec_name.c_str());
	sprintf(msg, "The vector %s (%.3f, %.3f, %.3f) must be a unit vector.", name, vec.x, vec.y, vec.z);
	
	return std::string(msg).c_str();
}

invalid_vector::~invalid_vector() throw() {}

/*****************************************/
/************* invalid_file **************/
/*****************************************/

invalid_file::invalid_file(std::string name) {
	file_name = name;
}

const char* invalid_file::what() const throw() {
	std::string msg = std::string("Could not find file");
	if (file_name.length() != 0)
		msg += std::string(" with name \'") + file_name + std::string("\'");
	msg += ".";
	return msg.c_str();
}

invalid_file::~invalid_file() throw() {}

/*****************************************/
/************* parallel_coords **************/
/*****************************************/

const char* parallel_coords::what() const throw() {
	return "The up and view_dir cannot be parallel.";
}

/*****************************************/
/************* invalid_fov ***************/
/*****************************************/

const char* invalid_fov::what() const throw() {
	return "The field of view must be strictly lesser than 180.";
}

/******************************************/
/************* invalid_dims ***************/
/******************************************/

const char* invalid_dims::what() const throw() {
	return "The dimensions of the image must be positive.";
}

/******************************************/
/********** invalid_scene_file ************/
/******************************************/

const char* invalid_scene_file::what() const throw() {
	return "The scene file is invalid.";
}

/******************************************/
/************ invalid_color ***************/
/******************************************/

const char* invalid_color::what() const throw() {
	return "The color is invalid.";
}