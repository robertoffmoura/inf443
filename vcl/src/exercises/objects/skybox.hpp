#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"

class Skybox {
public:
    void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);

private:
	GLuint skybox_texture_id;
	vcl::mesh_drawable skybox;

};