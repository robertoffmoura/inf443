#include "grass.hpp"
#include "terrain.hpp"
#include <random>

using namespace vcl;

void Grass::setup() {
	bill_grass_texture_id = texture_gpu( image_load_png("data/billboard_grass.png") );

	mesh grass_surface_cpu;
	grass_surface_cpu.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0, 0.4f}, {-0.2f, 0, 0.4f}};
	grass_surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
	grass_surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

	grass_surface = grass_surface_cpu;
	grass_surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
}

void Grass::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
	// Display two orthogonal billboards with static orientation
	// ********************************************************** //
	glBindTexture(GL_TEXTURE_2D, bill_grass_texture_id);
	const mat3 Identity = mat3::identity();
	const mat3 R = rotation_from_axis_angle_mat3({0,0,1}, 3.14f/2.0f); // orthogonal rotation

	grass_surface.uniform_parameter.translation = {0,0,0};

	for (size_t i=0; i < grass_position.size(); i++) {
		grass_surface.uniform_parameter.translation = grass_position[i] + vec3(0.0f, 0.0f, -0.03f);
		grass_surface.uniform_parameter.rotation = Identity;
		grass_surface.draw(shaders["mesh"], scene.camera);
		if (wireframe) {
			grass_surface.draw(shaders["wireframe"], scene.camera);
		}

		grass_surface.uniform_parameter.rotation = R;
		grass_surface.draw(shaders["mesh"], scene.camera);
		if (wireframe) {
			grass_surface.draw(shaders["wireframe"], scene.camera);
		}
	}


	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void Grass::set_bill_grass_position(gui_scene_structure& gui_scene, Terrain& terrain) {
	unsigned seed = 3;
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator(seed);
	size_t number_of_grass = 40;

	for (size_t i=0; i<number_of_grass; i++) {
		float u = distrib(generator);
		float v = distrib(generator);
		grass_position.push_back(terrain.evaluate_terrain(u,v, gui_scene));
	}
}