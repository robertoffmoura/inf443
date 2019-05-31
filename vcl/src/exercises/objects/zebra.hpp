#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"

class Zebra {
public:
    void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);

	vcl::timer_interval zebra_timer;

private:
	vcl::mesh_drawable_hierarchy hierarchy;
};