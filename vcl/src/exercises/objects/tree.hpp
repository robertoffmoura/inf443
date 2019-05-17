#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "mesh.hpp"
#include "gui.hpp"

class Terrain;

class Tree {
public:
	std::vector<vcl::vec3> tree_position;

	void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);

	void set_tree_position(gui_scene_structure& gui_scene, Terrain& terrain);
private:
	vcl::mesh_drawable_hierarchy tree;
};