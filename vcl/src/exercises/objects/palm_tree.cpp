#include "palm_tree.hpp"
#include "terrain.hpp"
#include <random>

using namespace vcl;
using std::vector;

mesh create_leaves(float angle, float r0, float r2, vec3 translation);
mesh create_curved_cylinder(float r_cylinder, float angle, float r0, float r2, vec3 translation);

void Palm_tree::setup() {
	float angle = 0.8f;
	float r0 = 1.0f;
	float r2 = 2.0f;
	float trunk_radius = 0.04f;
	float branch_r0 = 0.6f;
	float branch_r2 = 0.3f;
	float branch_angle = 1.0f;
	float r_top = 0.06f;
	float r_coconut = 0.03f;
	int n_leaves0 = 4;
	int n_leaves1 = 8;
	int n_leaves2 = 8;
	int n_coconuts = 6;
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator;

	// Create visual cylinder surface
	mesh_drawable trunk = create_curved_cylinder(trunk_radius, angle, r0, r2, {0, 0, 0});
	trunk.uniform_parameter.color = {0.815f, 0.729f, 0.623f};
	trunk.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

	mesh_drawable leaves = create_leaves(branch_angle, branch_r0, branch_r2, {0, 0, 0});
	leaves.uniform_parameter.color = {0.36f, 0.51f, 0.3f};
	leaves.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

	mesh_drawable top = mesh_primitive_sphere(r_top, {0,0,0}, 20, 20);
	top.uniform_parameter.color = {0.815f, 0.729f, 0.623f};
	top.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

	mesh_drawable coconut = mesh_primitive_sphere(r_coconut, {r_top,0,0}, 10, 10);
	coconut.uniform_parameter.color = {0.396f, 0.263f, 0.129f};
	coconut.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material


	palm_tree.add_element(trunk, "trunk", "root", {0,0,-0.1f});
	for (int i=0; i<n_leaves0; i++) {
		palm_tree.add_element(leaves, "leaves_up" + std::to_string(i), "trunk", {0, 0, 2*r2*sin(angle)},
		rotation_from_axis_angle_mat3({0,0,1}, i/(1.0*n_leaves0) *2*M_PI)
		*rotation_from_axis_angle_mat3({0,1,0}, -0.2f - 0.6f*distrib(generator)));
	}
	for (int i=0; i<n_leaves1; i++) {
		palm_tree.add_element(leaves, "leaves_medium" + std::to_string(i), "trunk", {0, 0, 2*r2*sin(angle)},
		rotation_from_axis_angle_mat3({0,0,1}, i/(1.0*n_leaves1) *2*M_PI)
		*rotation_from_axis_angle_mat3({0,1,0}, 0.2f - 0.4f*distrib(generator)));
	}
	for (int i=0; i<n_leaves2; i++) {
		palm_tree.add_element(leaves, "leaves_down" + std::to_string(i), "trunk", {0, 0, 2*r2*sin(angle)},
		rotation_from_axis_angle_mat3({0,0,1}, i/(1.0*n_leaves2) *2*M_PI)
		*rotation_from_axis_angle_mat3({0,1,0}, 0.2f +0.6f*distrib(generator)));
	}

	for (int i=0; i<n_coconuts; i++) {
		palm_tree.add_element(coconut, "coconut" + std::to_string(i), "trunk", {0, 0, 2*r2*sin(angle)},
		rotation_from_axis_angle_mat3({0,0,1}, i/(1.0*n_coconuts) *2*M_PI)
		*rotation_from_axis_angle_mat3({0,1,0}, -0.8 + 1.6f*distrib(generator)));
	}

	palm_tree.add_element(top, "top", "trunk", {0, 0, 2*r2*sin(angle)});

}

void Palm_tree::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
	/*
	palm_tree.draw(shaders["mesh"], scene.camera);
	if( wireframe ){ // wireframe if asked from the GUI
		palm_tree.draw(shaders["wireframe"], scene.camera);
	}
	*/
	for (size_t i=0; i < palm_tree_position.size(); i++) {
		palm_tree.translation("trunk") =  palm_tree_position[i] + vec3(0.0f, 0.0f, -0.05f);
		palm_tree.rotation("trunk") = rotation_from_axis_angle_mat3({0,0,1}, palm_tree_rotation[i]);
		palm_tree.draw(shaders["mesh"], scene.camera);
		if (wireframe) {
			palm_tree.draw(shaders["wireframe"], scene.camera);
		}
	}
}

mesh create_leaves(float angle, float r0, float r2, vec3 translation) {
	unsigned int N = 40;
	vec3 p0 = {r0*sin(angle), 0, -r2*cos(angle)};
	vec3 p1 = {r0*sin(angle), -r2*cos(angle), 0};
	vec3 p2 = {r0*sin(angle), r2*cos(angle), 0};
	p0 += translation;
	p1 += translation;
	p2 += translation;

	mesh branch; // temporary terrain storage (CPU only)
	branch.position.resize(3*N-2);

	for (size_t i=0; i<N; ++i) {
		const float a = -angle + i/(1.0*(N-1)) * 2*angle;
		vec3 position = {r0*sin(a), 0, r2*cos(a)};
		position += p0;
		branch.position[i] = position;
	}
	for (size_t i=0; i<N-1; ++i) {
		const float a = -angle + (0.5f+i)/(1.0*(N-1)) * 2*angle;
		vec3 position = {r0*sin(a), r2*cos(a), 0};
		position += p1;
		branch.position[N + i] = position;
	}
	for (size_t i=0; i<N-1; ++i) {
		const float a = -angle + (0.5f+i)/(1.0*(N-1)) * 2*angle;
		vec3 position = {r0*sin(a), -r2*cos(a), 0};
		position += p2;
		branch.position[2*N - 1 + i] = position;
	}

	// Generate triangle organization
	for (unsigned int i=0; i<N-1; ++i) {
		const index3 triangle  = {i, N + i,    i+1};
		const index3 triangle2 = {i, 2*N-1 + i, i+1};
		branch.connectivity.push_back(triangle);
		branch.connectivity.push_back(triangle2);
	}
	return branch;
}

mesh create_curved_cylinder(float r_cylinder, float angle, float r0, float r2, vec3 translation) {
	const unsigned int N = 20;
	const unsigned int N2 = 20;

	vec3 p = {-r0*cos(angle), 0, r2*sin(angle)};
	p += translation;

	mesh cylinder; // temporary terrain storage (CPU only)
	cylinder.position.resize(N2*N);

	// Fill cylinder geometry
	for (size_t i=0; i<N2; i++) {
		for (size_t ku=0; ku<N; ++ku) {
			const float a = -angle + i/(1.0*(N2-1)) * 2*angle;
			vec3 center = {r0*cos(a), 0, r2*sin(a)};

			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = sin(ku /(1.0*N) * 2*M_PI );
			const float v = cos(ku /(1.0*N) * 2*M_PI );
			vec3 offset = {r_cylinder * u, r_cylinder * v, 0};
			// Compute coordinates
			cylinder.position[i*N + ku] = p + center + offset;
		}
	}

	// Generate triangle organization
	for (unsigned int i=0; i<N2-1; i++) {
		for (unsigned int ku=0; ku<N-1; ++ku) {
			index3 triangle  = {i*N + ku,         (i+1)*N + ku,  i*N + ku + 1};
			index3 triangle2 = {(i+1)*N + ku + 1, i*N + ku + 1,  (i+1)*N + ku};
			cylinder.connectivity.push_back(triangle);
			cylinder.connectivity.push_back(triangle2);
		}
		index3 triangle  = {i*N + N-1, (i+1)*N + N-1,   i*N          };
		index3 triangle2 = {(i+1)*N  ,   i*N,           (i+1)*N + N-1};
		cylinder.connectivity.push_back(triangle);
		cylinder.connectivity.push_back(triangle2);
	}

	return cylinder;
}


void Palm_tree::set_tree_position(gui_scene_structure& gui_scene, Terrain& terrain) {
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator;
	size_t number_of_trees = 40;

	float tree_radius = 0.4f;
	float square_size = 0.8f; //approximately equal to 2 tree radii
	float terrain_size = 20.0f;
	size_t grid_len = terrain_size / square_size + 1;
	//bool grid[grid_len][grid_len];
	vector<vector<bool>> grid(grid_len, vector<bool>(grid_len));
	for(size_t i = 0; i < grid_len; i++){
		for(size_t j = 0; j < grid_len; j++){
			grid[i][j] = false;
		}
	}
	print_grid(grid);

	for (size_t i=0; i<number_of_trees; i++) {
		float u = distrib(generator);
		float v = distrib(generator);
		//vector<ivec2> neighbors = get_tree_neighbors(u, v);

		while (!valid_tree_position(u, v, grid, grid_len, terrain_size, tree_radius)) {
			u = distrib(generator);
			v = distrib(generator);
		}
		palm_tree_position.push_back(terrain.evaluate_terrain(0.25f + u/2.0f, 0.25f + v/2.0f, gui_scene));
		//tree_position.push_back({u, v, 0});
		update_tree_position_grid(u, v, grid, grid_len, terrain_size, tree_radius);
		palm_tree_rotation.push_back(2*M_PI*distrib(generator));
	}
	print_grid(grid);
}

bool Palm_tree::valid_grid_coordinates(int i, int j, size_t grid_len) {
	if (i < 0 || i >= (int) grid_len) return false;
	if (j < 0 || j >= (int) grid_len) return false;
	return true;
}

/*
vector<ivec2> get_tree_neighbors(float u, float v)
{
	vector<ivec2> result;
	int dx[] = {1, 1, -1, -1};
	int dy[] = {1, -1, 1, -1};
	float x;
	float y;
	int i;
	int j;
	for (int k=0; k<4; k++) {
		x = u * terrain_size + dx[k] * tree_radius;
		y = v * terrain_size + dy[k] * tree_radius;
		i = (x * grid_len) / terrain_size;
		j = (y * grid_len) / terrain_size;
		std::cout << i << ", " << j << std::endl;
		if (!valid_grid_coordinates(i, j, grid_len)) continue;
		if (grid[i][j] == false) continue;
		result.push_back(ivec2(i, j));
	}
	return result;
}
*/

bool Palm_tree::valid_tree_position(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius) {
	int dx[] = {1, 1, -1, -1};
	int dy[] = {1, -1, 1, -1};
	float x;
	float y;
	int i;
	int j;
	for (int k=0; k<4; k++) {
		x = u * terrain_size + dx[k] * tree_radius;
		y = v * terrain_size + dy[k] * tree_radius;
		i = (x * grid_len) / terrain_size;
		j = (y * grid_len) / terrain_size;
		if (!valid_grid_coordinates(i, j, grid_len)) continue;
		if (grid[i][j] == true) return false;
	}
	return true;
}

void Palm_tree::print_grid(vector<vector<bool>> &grid) {
	size_t grid_len = grid.size();
	for(size_t i = 0; i < grid_len; i++){
		for(size_t j = 0; j < grid_len; j++){
			std::cout << grid[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void Palm_tree::update_tree_position_grid(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius) {
	int dx[] = {1, 1, -1, -1};
	int dy[] = {1, -1, 1, -1};
	float x;
	float y;
	int i;
	int j;
	for (int k=0; k<4; k++) {
		x = u * terrain_size + dx[k] * tree_radius;
		y = v * terrain_size + dy[k] * tree_radius;
		i = x * grid_len / terrain_size;
		j = y * grid_len / terrain_size;
		if (!valid_grid_coordinates(i, j, grid_len)) continue;
		grid[i][j] = true;
	}
}