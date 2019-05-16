#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "mesh.hpp"

class Mushroom {
public:
	std::vector<vcl::vec3> mushroom_position;

	void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);
private:
	vcl::mesh_drawable_hierarchy mushroom;
};