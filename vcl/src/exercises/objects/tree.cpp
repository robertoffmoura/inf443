#include "tree.hpp"
#include "terrain.hpp"
#include <random>

using namespace vcl;
using std::vector;

mesh create_tree_foliage(float radius, float height, float z_offset);

bool valid_tree_position(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);
bool valid_grid_coordinates(int i, int j, size_t grid_len);
void update_tree_position_grid(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);

void print_grid(vector<vector<bool>> &grid);
//vector<ivec2> get_tree_neighbors(float u, float v);

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


void Tree::set_tree_position(gui_scene_structure& gui_scene, Terrain& terrain) {
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
		tree_position.push_back(terrain.evaluate_terrain(u,v, gui_scene));
		//tree_position.push_back({u, v, 0});
		update_tree_position_grid(u, v, grid, grid_len, terrain_size, tree_radius);
	}
	print_grid(grid);
}

bool valid_grid_coordinates(int i, int j, size_t grid_len) {
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

bool valid_tree_position(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius) {
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

void print_grid(vector<vector<bool>> &grid) {
	size_t grid_len = grid.size();
	for(size_t i = 0; i < grid_len; i++){
		for(size_t j = 0; j < grid_len; j++){
			std::cout << grid[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void update_tree_position_grid(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius) {
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