#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include <iostream>

class Penguin {
public:
    void setup(float scale);
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe, vcl::vec3 position, vcl::vec3 derivative);

	vcl::timer_interval penguin_timer;

	vcl::mesh_drawable_hierarchy hierarchy;
	private:
};