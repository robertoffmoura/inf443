#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "mesh.hpp"

class Tree {
public:
	std::vector<vcl::vec3> tree_position;

	void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);
private:
	vcl::mesh_drawable_hierarchy tree;
};