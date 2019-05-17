#include "flower.hpp"

using namespace vcl;

void Flower::setup() {
	bill_flower_texture_id = texture_gpu( image_load_png("data/billboard_redflowers.png") );
	mesh flower_surface_cpu;
	flower_surface_cpu.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0.4f, 0}, {-0.2f, 0.4f, 0}};
	flower_surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
	flower_surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

	flower_surface = flower_surface_cpu;
	flower_surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
}

void Flower::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
	// Enable use of alpha component as color blending for transparent elements
	//  new color = previous color + (1-alpha) current color
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

	// Display a billboard always facing the camera direction
	// ********************************************************** //
	glBindTexture(GL_TEXTURE_2D, bill_flower_texture_id);
	flower_surface.uniform_parameter.translation = {0,0,0};

	for (size_t i=0; i < flower_position.size(); i++) {
		flower_surface.uniform_parameter.translation = flower_position[i] + vec3(0.0f, 0.0f, -0.03f);
		flower_surface.uniform_parameter.rotation = scene.camera.orientation;
		flower_surface.draw(shaders["mesh"], scene.camera);
		if (wireframe) {
			flower_surface.draw(shaders["wireframe"], scene.camera);
		}
	}
	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}