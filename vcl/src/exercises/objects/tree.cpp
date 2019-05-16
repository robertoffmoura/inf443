#include "tree.hpp"

using namespace vcl;

mesh create_tree_foliage(float radius, float height, float z_offset);

void Tree::setup() {
	// Create visual cylinder surface
	mesh_drawable cylinder = create_cylinder(0.1f, 0.8f);
	cylinder.uniform_parameter.color = {0.396f, 0.263f, 0.129f};
	cylinder.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material
	tree.add_element(cylinder, "trunk", "root", {0,0,0});


	// Create visual cone surface
	//cone = create_cone(2.0f, 3.0f, 4.0f);
	//cone.uniform_parameter.color = {0.6f,0.85f,0.5f};
	//cone.uniform_parameter.shading.specular = 0.0f; // vec3(0.0f, 0.0f, 0.8f)non-specular terrain material

	// Create visual foliage surface
	mesh_drawable foliage = create_tree_foliage(0.4f, 0.6f, 0.24f);
	foliage.uniform_parameter.color = {0.36f, 0.51f, 0.3f};
	foliage.uniform_parameter.shading.specular = 0.0f;
	tree.add_element(foliage, "foliage", "trunk", {0.0f, 0.0f, 0.8f});
}

void Tree::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
	for (size_t i=0; i < tree_position.size(); i++) {
		tree.translation("trunk") =  tree_position[i] + vec3(0.0f, 0.0f, -0.05f);
		tree.draw(shaders["mesh"], scene.camera);
		if (wireframe) {
			tree.draw(shaders["wireframe"], scene.camera);
		}
	}
}

mesh create_tree_foliage(float radius, float height, float z_offset) {
	mesh m = create_cone(radius, height, 0);
	m.push_back( create_cone(radius, height, z_offset) );
	m.push_back( create_cone(radius, height, 2*z_offset) );

	return m;
}