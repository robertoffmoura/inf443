#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"

class Palm_tree {
public:
    void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);

private:
    vcl::mesh_drawable_hierarchy palm_tree;

};
