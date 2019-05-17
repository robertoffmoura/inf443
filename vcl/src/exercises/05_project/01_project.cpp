
#include "01_project.hpp"

#ifdef INF443_01_PROJECT

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
using std::vector;
#include <random>
#include <iostream>


float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v, const gui_scene_structure& gui_scene);
mesh create_terrain(const gui_scene_structure& gui_scene);

bool valid_tree_position(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);
bool valid_grid_coordinates(int i, int j, size_t grid_len);
void update_tree_position_grid(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);

void print_grid(vector<vector<bool>> &grid);
//vector<ivec2> get_tree_neighbors(float u, float v);

/** This function is called before the beginning of the animation loop
	It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& ) {
	// Load a texture image on GPU and stores its ID
	terrain_texture_id = texture_gpu( image_load_png("data/grass.png") );

	trajectory.setup();
	penguin.setup(0.5f);

	set_tree_position();
	set_mushroom_position();
	set_bill_grass_position();
	set_bill_flower_position();

	setup_terrain();
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

	draw_terrain(shaders, scene);
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

void scene_exercise::setup_terrain() {
	// Clear memory in case of pre-existing terrain
	terrain.data_gpu.clear();

	// Create visual terrain surface
	terrain = create_terrain(gui_scene);
	terrain.uniform_parameter.color = {1.0f, 1.0f, 1.0f};
	terrain.uniform_parameter.shading.specular = 0.0f;
}

void scene_exercise::draw_terrain(std::map<std::string,GLuint>& shaders, scene_structure& scene) {
	// Before displaying a textured surface: bind the associated texture id
	glBindTexture(GL_TEXTURE_2D, terrain_texture_id);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

	// Display terrain
	terrain.draw(shaders["mesh"], scene.camera);

	// After the surface is displayed it is safe to set the texture id to a white image
	//  Avoids to use the previous texture for another object
	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
	if( gui_scene.wireframe ){ // wireframe if asked from the GUI
		glPolygonOffset( 1.0, 1.0 );
		terrain.draw(shaders["wireframe"], scene.camera);
	}
}

void scene_exercise::mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods) {
	trajectory.mouse_click(scene, window, button, action, mods);
}

void scene_exercise::mouse_move(scene_structure& scene, GLFWwindow* window) {
	trajectory.mouse_move(scene, window);
}


float evaluate_terrain_z_h_p_sigma(float u, float v, float h, float sigma, vec2 p) {
	const float d = norm(vec2(u,v)-p)/sigma;
	const float z = h*std::exp(-d*d);
	return z;
}


// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v) {
	const vec2 u0 = {0.2f, 0.4f};
	const float h0 = 1.7f;
	const float sigma0 = 0.15f;

	const vec2 u1 = {0.7f, 0.7f};
	const float h1 = -1.3f;
	const float sigma1 = 0.25f;

	const vec2 u2 = {0.7f, 0.2f};
	const float h2 = 1.5f;
	const float sigma2 = 0.20f;

	float z = 0;
	z += evaluate_terrain_z_h_p_sigma(u, v, h0, sigma0, u0);
	z += evaluate_terrain_z_h_p_sigma(u, v, h1, sigma1, u1);
	z += evaluate_terrain_z_h_p_sigma(u, v, h2, sigma2, u2);

	return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v, const gui_scene_structure& gui_scene) {
	// get gui parameters
	const float scaling = gui_scene.scaling;
	const int octave = gui_scene.octave;
	const float persistency = gui_scene.persistency;
	const float height = gui_scene.height;

	// Evaluate Perlin noise
	const float noise = perlin(scaling*u, scaling*v, octave, persistency);

	// 3D vertex coordinates
	const float x = 20*(u-0.5f);
	const float y = 20*(v-0.5f);
	const float z = evaluate_terrain_z(u,v) + height*noise;

	return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain(const gui_scene_structure& gui_scene) {
	// Number of samples of the terrain is N x N
	const size_t N = 100;

	mesh terrain; // temporary terrain storage (CPU only)
	terrain.position.resize(N*N);
	terrain.color.resize(N*N);

	// Fill terrain geometry
	for(size_t ku=0; ku<N; ++ku)
	{
		for(size_t kv=0; kv<N; ++kv)
		{
			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku/(N-1.0f);
			const float v = kv/(N-1.0f);

			// get gui parameters
			const float scaling = gui_scene.scaling;
			const int octave = gui_scene.octave;
			const float persistency = gui_scene.persistency;

			// Evaluate Perlin noise
			const float noise = perlin(scaling*u, scaling*v, octave, persistency);

			const float c = 0.3f+0.7f*noise;

			terrain.color[kv+N*ku]  = {c,c,c,1.0f};

			// Compute coordinates
			terrain.position[kv+N*ku] = evaluate_terrain(u,v, gui_scene);
			terrain.texture_uv.push_back(vec2(15*u, 15*v));
		}
	}


	// Generate triangle organization
	//  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
	const unsigned int Ns = N;
	for(unsigned int ku=0; ku<Ns-1; ++ku)
	{
		for(unsigned int kv=0; kv<Ns-1; ++kv)
		{
			const unsigned int idx = kv + N*ku; // current vertex offset

			const index3 triangle_1 = {idx, idx+1+Ns, idx+1};
			const index3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

			terrain.connectivity.push_back(triangle_1);
			terrain.connectivity.push_back(triangle_2);
		}
	}

	return terrain;
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
		tree.tree_position.push_back(evaluate_terrain(u,v, gui_scene));
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
		mushroom.mushroom_position.push_back(evaluate_terrain(u,v, gui_scene));
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
		grass.grass_position.push_back(evaluate_terrain(u,v, gui_scene));
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
		flower.flower_position.push_back(evaluate_terrain(u,v, gui_scene));
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
