#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "gui.hpp"

class Terrain {
public:
	vcl::mesh_drawable terrain;
	vcl::mesh_drawable sand_terrain;
	GLuint terrain_texture_id;
	GLuint sand_terrain_texture_id;

	void setup(gui_scene_structure& gui_scene);
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);

	vcl::mesh create_terrain(const gui_scene_structure& gui_scene);
	vcl::mesh create_sand_terrain(const gui_scene_structure& gui_scene);
	float evaluate_terrain_z_h_p_sigma(float u, float v, float h, float sigma, vcl::vec2 p);
	float evaluate_terrain_z(float u, float v);
	vcl::vec3 evaluate_terrain(float u, float v, const gui_scene_structure& gui_scene);
private:

};