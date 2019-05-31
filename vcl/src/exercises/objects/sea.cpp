#include "sea.hpp"
#include "terrain.hpp"
#include <random>

using namespace vcl;

void Sea::setup() {
	sea_texture_id = texture_gpu( image_load_png("data/sea.png") );

	mesh sea_surface_cpu;
	sea_surface_cpu.position     = {{-20.0f, -20.0f, 0}, { 20.0f, -20.0f, 0}, { 20.0f, 20.0f, 0}, {-20.0f, 20.0f, 0}};
	sea_surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
	sea_surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

	sea_surface = sea_surface_cpu;
	sea_surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
}

void Sea::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
	
	glBindTexture(GL_TEXTURE_2D, sea_texture_id);

	sea_surface.uniform_parameter.translation = {0,0,1.0f};

	sea_surface.draw(shaders["mesh"], scene.camera);
	if (wireframe) {
		sea_surface.draw(shaders["wireframe"], scene.camera);
	}

	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}