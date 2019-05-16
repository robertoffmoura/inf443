#include "mushroom.hpp"

using namespace vcl;

void Mushroom::setup() {
	// Create visual cylinder surface
	mesh_drawable mushroom_cylinder = create_cylinder(0.02f, 0.08f);
	mushroom_cylinder.uniform_parameter.color = {0.9f, 0.9f, 0.9f};
	mushroom_cylinder.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

	mesh_drawable mushroom_cone = create_cone(0.08f, 0.05f, 0.08f);
	mushroom_cone.uniform_parameter.color = {0.9f,0.0f,0.0f};
	mushroom_cone.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

	mushroom.add_element(mushroom_cylinder, "body", "root");
	mushroom.add_element(mushroom_cone, "cone", "body");
}

void Mushroom::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
	for (size_t i=0; i < mushroom_position.size(); i++) {
		mushroom.translation("body") =  mushroom_position[i] + vec3(0.0f, 0.0f, -0.02f);
		mushroom.draw(shaders["mesh"], scene.camera);
		if (wireframe) { // wireframe if asked from the GUI
			mushroom.draw(shaders["wireframe"], scene.camera);
		}
	}
}
