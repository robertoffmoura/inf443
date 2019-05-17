#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"

class Flower {
public:
	GLuint bill_flower_texture_id;
	std::vector<vcl::vec3> flower_position;
	vcl::mesh_drawable flower_surface;

	void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);
private:

};