#include <stdexcept>
#include <string>

#include "geometry.hpp"

class invalid_vector : public std::exception {
	Vec3 vec;
	std::string vec_name;
public:
	invalid_vector(Vec3 v, std::string vec_name=std::string(""));
	const char* what() const throw();
	~invalid_vector() throw();
};


class invalid_file : public std::exception {
	std::string file_name;
public:
	invalid_file(std::string file_name=std::string(""));
	const char* what() const throw();
	~invalid_file() throw();
};

class parallel_coords : public std::exception {
public:
	const char * what() const throw();
};

class invalid_fov : public std::exception {
public:
	const char* what() const throw();
};

class invalid_dims : public std::exception {
public:
	const char* what() const throw();
};

class invalid_scene_file : public std::exception {
public:
	const char* what() const throw();
};

class invalid_color : public std::exception {
public:
	const char* what() const throw();
};

class invalid_constant : public std::exception {
public:
	const char* what() const throw();
};