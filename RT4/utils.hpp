#include <string>
#include <vector>

#include "geometry.hpp"
#include "lights.hpp"
#include "params.hpp"

Params parse_input(std::string filename, std::vector<Surface*>& surfaces, std::vector<LightSource*>& lights, std::vector<Texture*>& textures);
std::string remove_ext(std::string mystr);
std::string get_path(std::string full_path);