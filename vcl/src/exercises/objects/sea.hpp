#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "gui.hpp"

class Sea {
public:
	GLuint sea_texture_id;
	vcl::mesh_drawable sea_surface;

	void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);
private:

};