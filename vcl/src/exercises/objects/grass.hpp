#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "gui.hpp"

class Terrain;

class Grass {
public:
	GLuint bill_grass_texture_id;
	std::vector<vcl::vec3> grass_position;
	vcl::mesh_drawable grass_surface;

	void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);

	void set_bill_grass_position(gui_scene_structure& gui_scene, Terrain& terrain);
private:

};