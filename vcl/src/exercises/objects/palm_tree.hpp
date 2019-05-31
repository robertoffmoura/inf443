#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "gui.hpp"

using std::vector;

class Terrain;

class Palm_tree {
public:
	std::vector<vcl::vec3> palm_tree_position;
	std::vector<float> palm_tree_rotation;


    void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe);

	bool valid_tree_position(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);
	bool valid_grid_coordinates(int i, int j, size_t grid_len);
	void update_tree_position_grid(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);

	void print_grid(vector<vector<bool>> &grid);

	void set_tree_position(gui_scene_structure& gui_scene, Terrain& terrain);
private:
    vcl::mesh_drawable_hierarchy palm_tree;

};
