
#include "01_project.hpp"

#ifdef INF443_01_PROJECT

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
using std::vector;
#include <random>
#include <iostream>


bool valid_tree_position(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);
bool valid_grid_coordinates(int i, int j, size_t grid_len);
void update_tree_position_grid(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);

void print_grid(vector<vector<bool>> &grid);
//vector<ivec2> get_tree_neighbors(float u, float v);

/** This function is called before the beginning of the animation loop
	It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& ) {
	trajectory.setup();
	penguin.setup(0.5f);

	set_tree_position();
	set_mushroom_position();
	set_bill_grass_position();
	set_bill_flower_position();

	terrain.setup(gui_scene);
	tree.setup();
	mushroom.setup();
	grass.setup();
	flower.setup();
	skybox.setup();
	palm_tree.setup();

	// Setup initial camera mode and position
	scene.camera.camera_type = camera_control_spherical_coordinates;
	scene.camera.scale = 0.01f;
	scene.camera.apply_rotation(0,0,0,1.2f);
	scene.camera.apply_translation_orthogonal_to_screen_plane(-16.0f);
}


/** This function is called at each frame of the animation loop.
	It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure&) {
	//trajectory.timer.update();
	set_gui();

	glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe
	glPolygonOffset( 1.0, 1.0 );

	terrain.draw(shaders, scene, gui_scene.wireframe);
	tree.draw(shaders, scene, gui_scene.wireframe);
	mushroom.draw(shaders, scene, gui_scene.wireframe);
	palm_tree.draw(shaders, scene, gui_scene.wireframe);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);
	flower.draw(shaders, scene, gui_scene.wireframe);
	skybox.draw(shaders, scene, gui_scene.wireframe);
	grass.draw(shaders, scene, gui_scene.wireframe);
	glDepthMask(true);

	trajectory.draw(shaders, scene, gui_scene.trajectory);
	penguin.draw(shaders, scene, gui_scene.wireframe, trajectory.position(), trajectory.position_derivative());
}

void scene_exercise::mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods) {
	trajectory.mouse_click(scene, window, button, action, mods);
}

void scene_exercise::mouse_move(scene_structure& scene, GLFWwindow* window) {
	trajectory.mouse_move(scene, window);
}


void scene_exercise::set_tree_position() {
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
		tree.tree_position.push_back(terrain.evaluate_terrain(u,v, gui_scene));
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

void scene_exercise::set_mushroom_position() {
	unsigned seed = 2;
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator(seed);
	size_t number_of_mushrooms = 40;

	for (size_t i=0; i<number_of_mushrooms; i++) {
		float u = distrib(generator);
		float v = distrib(generator);
		mushroom.mushroom_position.push_back(terrain.evaluate_terrain(u,v, gui_scene));
	}
}

void scene_exercise::set_bill_grass_position() {
	unsigned seed = 3;
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator(seed);
	size_t number_of_grass = 40;

	for (size_t i=0; i<number_of_grass; i++) {
		float u = distrib(generator);
		float v = distrib(generator);
		grass.grass_position.push_back(terrain.evaluate_terrain(u,v, gui_scene));
	}
}

void scene_exercise::set_bill_flower_position() {
	unsigned seed = 4;
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator(seed);
	size_t number_of_flowers = 40;

	for (size_t i=0; i<number_of_flowers; i++) {
		float u = distrib(generator);
		float v = distrib(generator);
		flower.flower_position.push_back(terrain.evaluate_terrain(u,v, gui_scene));
	}
}

void scene_exercise::set_gui() {
	ImGui::Checkbox("Wireframe", &gui_scene.wireframe);

	ImGui::Separator();
	/*
	ImGui::Text("Perlin parameters");

	float height_min = 0.1f;
	float height_max = 2.0f;
	if( ImGui::SliderScalar("Height", ImGuiDataType_Float, &gui_scene.height, &height_min, &height_max) )
		update_terrain();

	float scaling_min = 0.1f;
	float scaling_max = 10.0f;
	if( ImGui::SliderScalar("(u,v) Scaling", ImGuiDataType_Float, &gui_scene.scaling, &scaling_min, &scaling_max) )
		update_terrain();

	int octave_min = 1;
	int octave_max = 10;
	if( ImGui::SliderScalar("Octave", ImGuiDataType_S32, &gui_scene.octave, &octave_min, &octave_max) )
		update_terrain();

	float persistency_min = 0.1f;
	float persistency_max = 0.9f;
	if( ImGui::SliderScalar("Persistency", ImGuiDataType_Float, &gui_scene.persistency, &persistency_min, &persistency_max) )
		update_terrain();

	*/
	ImGui::SliderFloat("Time", &trajectory.timer.t, trajectory.timer.t_min, trajectory.timer.t_max);

	const float timek_scale_min = 0.1f;
	const float timek_scale_max = 3.0f;
	ImGui::SliderFloat("Time scale", &trajectory.timer.scale, timek_scale_min, timek_scale_max);

	if( ImGui::Button("Print Keyframe") )
	{
		std::cout<<"keyframe_position={";
		for(size_t k=0; k<trajectory.keyframe_position.size(); ++k)
		{
			const vec3& p = trajectory.keyframe_position[k];
			std::cout<< "{"<<p.x<<"f,"<<p.y<<"f,"<<p.z<<"f}";
			if(k<trajectory.keyframe_position.size()-1)
				std::cout<<", ";
		}
		std::cout<<"}"<<std::endl;
	}


	ImGui::SliderFloat("Penguin Time", &penguin.penguin_timer.t, penguin.penguin_timer.t_min, penguin.penguin_timer.t_max);

	const float penguin_time_scale_min = 0.1f;
	const float penguin_time_scale_max = 3.0f;
	ImGui::SliderFloat("Penguin Time scale", &penguin.penguin_timer.scale, penguin_time_scale_min, penguin_time_scale_max);

	ImGui::Checkbox("Trajectory", &gui_scene.trajectory);
}


#endif
