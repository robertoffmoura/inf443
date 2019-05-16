
#include "09_modelisation.hpp"

#ifdef INF443_09_MODELISATION

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
using std::vector;
#include <random>
#include <iostream>


float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v, const gui_scene_structure& gui_scene);
mesh create_terrain(const gui_scene_structure& gui_scene);

vec3 evaluate_cylinder(float u, float v, int ku, float radius, float height);
mesh create_cylinder(float radius, float height);

mesh create_cone(float radius, float height, float z_offset);
mesh create_tree_foliage(float radius, float height, float z_offset);

bool valid_tree_position(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);
bool valid_grid_coordinates(int i, int j, size_t grid_len);
void update_tree_position_grid(float u, float v, vector<vector<bool>> &grid, size_t grid_len, float terrain_size, float tree_radius);

void print_grid(vector<vector<bool>> &grid);
//vector<ivec2> get_tree_neighbors(float u, float v);

mesh mesh_foot(float scale);
mesh mesh_primitive_quad_scale(float scale, const vec3& p00, const vec3& p10, const vec3& p11, const vec3& p01);
mesh mesh_primitive_ellipsoid(float radius0, float radius1, float radius2, const vec3& p0, size_t Nu, size_t Nv);
mesh create_skybox();

static size_t index_at_value(float t, const std::vector<float>& vt);

static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);
static vec3 cardinal_spline_interpolation_derivative(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);

static vec3 cardinal_spline_interpolation(float t, std::vector<vcl::vec3> keyframe_position, std::vector<float> keyframe_time);
static vec3 cardinal_spline_interpolation_derivative(float t, std::vector<vcl::vec3> keyframe_position, std::vector<float> keyframe_time);

/** This function is called before the beginning of the animation loop
	It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& ) {
	// Load a texture image on GPU and stores its ID
	terrain_texture_id = texture_gpu( image_load_png("data/grass.png") );
	bill_grass_texture_id = texture_gpu( image_load_png("data/billboard_grass.png") );
	bill_flower_texture_id = texture_gpu( image_load_png("data/billboard_redflowers.png") );
	//sky_texture_id = texture_gpu( image_load_png("data/skybox3.png") );
	skybox_texture_id = texture_gpu( image_load_png("data/skybox2.png") );

	create_trajectory();
	create_penguin(0.5f);

	update_terrain();
	update_tree_position();
	update_mushroom_position();
	update_bill_grass_position();
	update_bill_flower_position();

	setup_terrain();
	setup_tree();
	setup_mushroom();
	setup_grass();
	setup_flower();
	setup_sky();


	// Setup initial camera mode and position
	scene.camera.camera_type = camera_control_spherical_coordinates;
	scene.camera.scale = 0.01f;
	scene.camera.apply_rotation(0,0,0,1.2f);
	scene.camera.apply_translation_orthogonal_to_screen_plane(-16.0f);
}



/** This function is called at each frame of the animation loop.
	It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){
	timer.update();
	set_gui();

	glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe
	glPolygonOffset( 1.0, 1.0 );

	draw_terrain(shaders, scene);
	draw_tree(shaders, scene);
	draw_mushroom(shaders, scene);
	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);
	draw_flower(shaders, scene);
	draw_sky(shaders, scene);
	draw_grass(shaders, scene);
	glDepthMask(true);

	draw_trajectory(shaders, scene);
	draw_penguin(shaders, scene);
}


void scene_exercise::create_penguin(float scale) {
	const float r_head = 0.25f * scale;
	const float r_eye = 0.05f * scale;
	const float r_pupil = 0.01f * scale;
	const float r_body0 = 0.32f * scale;
	const float r_body1 = 0.5f * scale;
	const float r_body2 = 0.22f * scale;
	const float foot_scale = 0.6f * scale;
	const float arm = 1.0f * scale;

	mesh_drawable neck = mesh_primitive_sphere(0.01f,{0,0,0},20,20);

	mesh_drawable body = mesh_primitive_ellipsoid(r_body0, r_body1, r_body2, {0,0,0},40,40);
	body.uniform_parameter.color = {0,0,0};

	mesh_drawable belly = mesh_primitive_ellipsoid(0.8f*r_body0, 0.8f*r_body1, 0.8f*r_body2, {0,0,-0.3636f*r_body2},40,40);
	belly.uniform_parameter.color = {1.0f,1.0f,1.0f};

	mesh_drawable head = mesh_primitive_sphere(r_head,{0,0,0},40,40);
	head.uniform_parameter.color = {0,0,0};

	mesh_drawable eye = mesh_primitive_sphere(r_eye,{0,0,0},20,20);
	eye.uniform_parameter.color = {1.0f,1.0f,1.0f};
	mesh_drawable pupil = mesh_primitive_sphere(r_pupil,{0,0,0},20,20);
	pupil.uniform_parameter.color = {0,0,0};

	mesh_drawable beak = mesh_primitive_cone(0.52f*r_head,  {0,0,0}, {0,1.2f*r_head,0}, 20, 20);
	beak.uniform_parameter.color = {1.0f, 0.7f, 0.4f};

	mesh_drawable arm_left = mesh_primitive_quad_scale(arm, {0.2f,0.3f,0}, {0.4f,0.2f,0}, {0.4f,0,0}, {0.2f,-0.1f,0});
	mesh_drawable arm_lower_left = mesh_primitive_quad_scale(arm, {0,0.2f,0}, {0.2f,0.1f,0}, {0.2f,0,0}, {0,0,0});
	mesh_drawable hand_left = mesh_primitive_quad_scale(arm, {0,0.1f,0}, {0.03f,0.07f,0}, {0.03f,0.03f,0}, {0,0,0});

	mesh_drawable arm_right = mesh_primitive_quad_scale(arm, {-0.2f,0.3f,0}, {-0.4f,0.2f,0}, {-0.4f,0,0}, {-0.2f,-0.1f,0});
	mesh_drawable arm_lower_right = mesh_primitive_quad_scale(arm, {-0,0.2f,0}, {-0.2f,0.1f,0}, {-0.2f,0,0}, {-0,0,0});
	mesh_drawable hand_right = mesh_primitive_quad_scale(arm, {-0,0.1f,0}, {-0.03f,0.07f,0}, {-0.03f,0.03f,0}, {0,0,0});

	arm_left.uniform_parameter.color = {0,0,0};
	arm_lower_left.uniform_parameter.color = {0,0,0};
	hand_left.uniform_parameter.color = {0,0,0};
	arm_right.uniform_parameter.color = {0,0,0};
	arm_lower_right.uniform_parameter.color = {0,0,0};
	hand_right.uniform_parameter.color = {0,0,0};

	mesh_drawable foot = mesh_foot(foot_scale);
	foot.uniform_parameter.color = {1.0f, 0.7f, 0.4f};


	hierarchy.add_element(body, "body", "root", {0,0,0});
	hierarchy.add_element(neck, "neck", "body", {0,0.8f*r_body1,0});
	hierarchy.add_element(head, "head", "neck", {0,0,1.2f*r_head});
	hierarchy.add_element(belly, "belly", "body", {0,0,0});
	hierarchy.add_element(foot, "foot_left", "body", {0.625f*r_body0,-0.8f*r_body1,0}, rotation_from_axis_angle_mat3({0,1,0}, 0.5f));
	hierarchy.add_element(foot, "foot_right", "body", {-0.625f*r_body0,-0.8f*r_body1,0}, rotation_from_axis_angle_mat3({0,1,0}, -0.5f));

	hierarchy.add_element(eye, "eye_left", "head",{r_head/3,r_head/1.5f,r_head/2});
	hierarchy.add_element(eye, "eye_right", "head",{-r_head/3,r_head/1.5f,r_head/2});
	hierarchy.add_element(pupil, "pupil_left", "head",{1.2f*r_head/3, 1.2f*r_head/1.5f, 1.2f*r_head/2});
	hierarchy.add_element(pupil, "pupil_right", "head",{-1.2f*r_head/3, 1.2f*r_head/1.5f, 1.2f*r_head/2});
	hierarchy.add_element(beak, "beak", "head", {0,r_head/1.5f,0});

	hierarchy.add_element(arm_left, "arm_left", "body", {0, -0.2f*r_body1, 0});
	hierarchy.add_element(arm_lower_left, "arm_lower_left", "arm_left", {0.4f*arm,0,0});
	hierarchy.add_element(hand_left, "hand_left", "arm_lower_left", {0.2f*arm,0,0});

	hierarchy.add_element(arm_right, "arm_right", "body", {0, -0.1f*r_body1, 0});
	hierarchy.add_element(arm_lower_right, "arm_lower_right", "arm_right", {-0.4f*arm,0,0});
	hierarchy.add_element(hand_right, "hand_right", "arm_lower_right", {-0.2f*arm,0,0});

	penguin_timer.scale = 0.8f;
}

void scene_exercise::create_trajectory() {
	// Initial Keyframe data
	//keyframe_position = {{0,0,0}, {3,0,0}, {3,3,0}, {6,3,0}, {9,3,0}, {9,0,0}, {12,0,0}, {12,-3,0}, {0,-3,0}, {0,0,0}, {3,0,0}, {3,3,0}};
	const size_t N = 9;
	for (size_t i=0; i<N+3; i++) {
		const float u = 3*sin(i /(1.0*N) * 2*M_PI );
		const float v = 3*cos(i /(1.0*N) * 2*M_PI );
		keyframe_position.push_back({u, v, 2});
	}
	keyframe_time = {0,1,2,3,4,5,6,7,8,9,10,11};
	// Set timer bounds
	//  To ease spline interpolation of a closed curve time \in [t_1,t_{N-2}]
	timer.t_min = keyframe_time[1];
	timer.t_max = keyframe_time[keyframe_time.size()-2];
	timer.t = timer.t_min;

	// Prepare the visual elements
	//surface = mesh_primitive_sphere();
	//surface.uniform_parameter.color   = {0,0,1};
	//surface.uniform_parameter.scaling = 0.08f;

	sphere = mesh_primitive_sphere();
	sphere.uniform_parameter.color = {1,1,1};
	sphere.uniform_parameter.scaling = 0.05f;

	segment_drawer.init();

	trajectory = curve_dynamic_drawable(100); // number of steps stroed in the trajectory
	trajectory.uniform_parameter.color = {0,0,1};

	picked_object=-1;
}

void scene_exercise::setup_terrain() {
	// Create visual terrain surface
	//terrain = create_terrain();
	//terrain.uniform_parameter.color = {0.6f, 0.85f, 0.5f};
	terrain.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material
}

void scene_exercise::setup_tree() {
	// Create visual cylinder surface
	cylinder = create_cylinder(0.1f, 0.8f);
	cylinder.uniform_parameter.color = {0.396f, 0.263f, 0.129f};
	cylinder.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

	// Create visual cone surface
	//cone = create_cone(2.0f, 3.0f, 4.0f);
	//cone.uniform_parameter.color = {0.6f,0.85f,0.5f};
	//cone.uniform_parameter.shading.specular = 0.0f; // vec3(0.0f, 0.0f, 0.8f)non-specular terrain material

	// Create visual foliage surface
	foliage = create_tree_foliage(0.4f, 0.6f, 0.24f);
	foliage.uniform_parameter.color = {0.36f, 0.51f, 0.3f};
	foliage.uniform_parameter.shading.specular = 0.0f;
}

void scene_exercise::setup_mushroom() {
	// Create visual cylinder surface
	mushroom_cylinder = create_cylinder(0.02f, 0.08f);
	mushroom_cylinder.uniform_parameter.color = {0.9f, 0.9f, 0.9f};
	mushroom_cylinder.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

	mushroom_cone = create_cone(0.08f, 0.05f, 0.08f);
	mushroom_cone.uniform_parameter.color = {0.9f,0.0f,0.0f};
	mushroom_cone.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material
}

void scene_exercise::setup_grass() {
	mesh grass_surface_cpu;
	grass_surface_cpu.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0, 0.4f}, {-0.2f, 0, 0.4f}};
	grass_surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
	grass_surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

	grass_surface = grass_surface_cpu;
	grass_surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
}

void scene_exercise::setup_flower() {
	mesh flower_surface_cpu;
	flower_surface_cpu.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0.4f, 0}, {-0.2f, 0.4f, 0}};
	flower_surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
	flower_surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

	flower_surface = flower_surface_cpu;
	flower_surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
}

void scene_exercise::setup_sky() {
	skybox = create_skybox();
	skybox.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
	skybox.uniform_parameter.translation = {1, 1, 1};
	skybox.uniform_parameter.scaling = 150;

	/*
	mesh sky_surface_cpu;
	sky_surface_cpu.position     = {{-0.2f,-0.2f,0}, { 0.2f,-0.2f,0}, { 0.2f, 0.2f, 0}, {-0.2f, 0.2f, 0}};
	sky_surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
	sky_surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

	sky_surface = sky_surface_cpu;
	sky_surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
	*/
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

void scene_exercise::draw_tree(std::map<std::string,GLuint>& shaders, scene_structure& scene) {
	for (size_t i=0; i < tree_position.size(); i++) {
		cylinder.uniform_parameter.translation = tree_position[i] + vec3(0.0f, 0.0f, -0.05f);
		cylinder.draw(shaders["mesh"], scene.camera);
		foliage.uniform_parameter.translation = tree_position[i] + vec3(0.0f, 0.0f, -0.05f) + vec3(0.0f, 0.0f, 0.8f);
		foliage.draw(shaders["mesh"], scene.camera);
	}
	if( gui_scene.wireframe ){ // wireframe if asked from the GUI
		for (size_t i=0; i< tree_position.size(); i++) {
			cylinder.uniform_parameter.translation = tree_position[i] + vec3(0.0f, 0.0f, -0.05f);
			cylinder.draw(shaders["wireframe"], scene.camera);
			foliage.uniform_parameter.translation = tree_position[i] + vec3(0.0f, 0.0f, -0.05f) + vec3(0.0f, 0.0f, 0.8f);
			foliage.draw(shaders["wireframe"], scene.camera);
		}
	}
}

void scene_exercise::draw_mushroom(std::map<std::string,GLuint>& shaders, scene_structure& scene) {
	for (size_t i=0; i < mushroom_position.size(); i++) {
		mushroom_cylinder.uniform_parameter.translation = mushroom_position[i] + vec3(0.0f, 0.0f, -0.02f);
		mushroom_cylinder.draw(shaders["mesh"], scene.camera);
		mushroom_cone.uniform_parameter.translation = mushroom_position[i] + vec3(0.0f, 0.0f, -0.02f);
		mushroom_cone.draw(shaders["mesh"], scene.camera);
	}

	if( gui_scene.wireframe ){ // wireframe if asked from the GUI
		for (size_t i=0; i < mushroom_position.size(); i++) {
			mushroom_cylinder.uniform_parameter.translation = mushroom_position[i] + vec3(0.0f, 0.0f, -0.02f);
			mushroom_cylinder.draw(shaders["wireframe"], scene.camera);
			mushroom_cone.uniform_parameter.translation = mushroom_position[i] + vec3(0.0f, 0.0f, -0.02f);
			mushroom_cone.draw(shaders["wireframe"], scene.camera);
		}
	}
}

void scene_exercise::draw_flower(std::map<std::string,GLuint>& shaders, scene_structure& scene) {
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
		if(gui_scene.wireframe)
			flower_surface.draw(shaders["wireframe"], scene.camera);
	}
	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void scene_exercise::draw_sky(std::map<std::string,GLuint>& shaders, scene_structure& scene) {
	// Display a billboard always facing the camera direction
	// ********************************************************** //
	glBindTexture(GL_TEXTURE_2D, skybox_texture_id);

	skybox.uniform_parameter.translation = scene.camera.camera_position() + vec3(0,0,-25.0f);

	skybox.draw(shaders["mesh"], scene.camera);
	if(gui_scene.wireframe)
		skybox.draw(shaders["wireframe"], scene.camera);

	/*
	glBindTexture(GL_TEXTURE_2D, sky_texture_id);

	sky_surface.uniform_parameter.translation = {0,0,0};
	sky_surface.uniform_parameter.translation = -30*scene.camera.orientation.col(2);
	sky_surface.uniform_parameter.scaling = 100 + scene.camera.scale*3.5f;
	sky_surface.uniform_parameter.rotation = scene.camera.orientation;
	//std::cout << scene.camera.orientation.col(2) << std::endl;
	sky_surface.draw(shaders["mesh"], scene.camera);
	if(gui_scene.wireframe)
		sky_surface.draw(shaders["wireframe"], scene.camera);
	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
	*/
}

void scene_exercise::draw_grass(std::map<std::string,GLuint>& shaders, scene_structure& scene) {
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
		if(gui_scene.wireframe)
			grass_surface.draw(shaders["wireframe"], scene.camera);

		grass_surface.uniform_parameter.rotation = R;
		grass_surface.draw(shaders["mesh"], scene.camera);
		if(gui_scene.wireframe)
			grass_surface.draw(shaders["wireframe"], scene.camera);
	}


	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void scene_exercise::draw_penguin(std::map<std::string,GLuint>& shaders, scene_structure& scene) {
	penguin_timer.update();
	//set_gui();
	const float t = timer.t;
	const float pt = penguin_timer.t;

	hierarchy.translation("body") =  {0,0,0.06f*(1-std::sin(2*3.14f*pt))};

	//hierarchy.translation("head") = {0,1,0};
	hierarchy.rotation("neck") = rotation_from_axis_angle_mat3({1,0,0}, -1 + 0.4f*std::sin(2*3.14f*(pt-0.4f)) );
	//hierarchy.translation("head") = {0,0,0};

	hierarchy.rotation("foot_left") = rotation_from_axis_angle_mat3({1,0,0}, -0.55f*std::sin(4*3.14f*(pt-0.4f)));
	hierarchy.rotation("foot_right") = rotation_from_axis_angle_mat3({1,0,0}, 0.55f*std::sin(4*3.14f*(pt-0.4f)));

	hierarchy.rotation("arm_left") = rotation_from_axis_angle_mat3({0,1,0}, 0.55f*std::sin(2*3.14f*(pt-0.4f)) );
	hierarchy.rotation("arm_lower_left") = rotation_from_axis_angle_mat3({0,1,0}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );
	hierarchy.rotation("hand_left") = rotation_from_axis_angle_mat3({0,1,0}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );

	hierarchy.rotation("arm_right") = rotation_from_axis_angle_mat3({0,-1,0}, 0.55f*std::sin(2*3.14f*(pt-0.4f)) );
	hierarchy.rotation("arm_lower_right") = rotation_from_axis_angle_mat3({0,-1,0}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );
	hierarchy.rotation("hand_right") = rotation_from_axis_angle_mat3({0,-1,0}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );

	const vec3 p = cardinal_spline_interpolation(t, keyframe_position, keyframe_time);
	hierarchy.translation("body") += p;
	const vec3 d = cardinal_spline_interpolation_derivative(t, keyframe_position, keyframe_time);
	//hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2)*rotation_between_vector_mat3({0,0,1}, d);
	//hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2)*rotation_between_vector_mat3({0,1,0}, d);
	hierarchy.rotation("body") = rotation_between_vector_mat3({0,1,0}, d);

	//std::cout << d << std::endl;

	hierarchy.draw(shaders["mesh"], scene.camera);
	if(gui_scene.wireframe)
		hierarchy.draw(shaders["wireframe"], scene.camera);
}


void scene_exercise::draw_trajectory(std::map<std::string,GLuint>& shaders, scene_structure& scene) {
	if(!gui_scene.trajectory) return;
	const float t = timer.t;
	// ********************************************* //
	// Compute interpolated position at time t
	// ********************************************* //

	// Assume a closed curve trajectory
	const size_t N = keyframe_time.size();


	const vec3 p = cardinal_spline_interpolation(t, keyframe_position, keyframe_time);
	trajectory.add_point(p);


	// Draw current position
	//surface.uniform_parameter.translation = p;
	//surface.draw(shaders["mesh"],scene.camera);


	// Draw sphere at each keyframe position
	for(size_t k=0; k<N; ++k)
	{
		const vec3& p_keyframe = keyframe_position[k];
		sphere.uniform_parameter.translation = p_keyframe;
		sphere.draw(shaders["mesh"],scene.camera);
	}


	// Draw segments between each keyframe
	for(size_t k=0; k<keyframe_position.size()-1; ++k)
	{
		const vec3& pa = keyframe_position[k];
		const vec3& pb = keyframe_position[k+1];

		segment_drawer.uniform_parameter.p1 = pa;
		segment_drawer.uniform_parameter.p2 = pb;
		segment_drawer.draw(shaders["segment_im"], scene.camera);
	}

	// Draw moving point trajectory
	trajectory.draw(shaders["curve"], scene.camera);

	// Draw selected sphere in red
	if( picked_object!=-1 )
	{
		const vec3& p_keyframe = keyframe_position[picked_object];
		sphere.uniform_parameter.color = vec3(1,0,0);
		sphere.uniform_parameter.scaling = 0.06f;
		sphere.uniform_parameter.translation = p_keyframe;
		sphere.draw(shaders["mesh"],scene.camera);
		sphere.uniform_parameter.color = vec3(1,1,1);
		sphere.uniform_parameter.scaling = 0.05f;
	}
}

void scene_exercise::mouse_click(scene_structure& scene, GLFWwindow* window, int , int action, int )
{
	// Mouse click is used to select a position of the control polygon
	// ******************************************************************** //

	// Window size
	int w=0;
	int h=0;
	glfwGetWindowSize(window, &w, &h);

	// Current cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Convert pixel coordinates to relative screen coordinates between [-1,1]
	const float x = 2*float(xpos)/float(w)-1;
	const float y = 1-2*float(ypos)/float(h);

	// Check if shift key is pressed
	const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

	if(action==GLFW_PRESS && key_shift)
	{
		// Create the 3D ray passing by the selected point on the screen
		const ray r = picking_ray(scene.camera, x,y);

		// Check if this ray intersects a position (represented by a sphere)
		//  Loop over all positions and get the intersected position (the closest one in case of multiple intersection)
		const size_t N = keyframe_position.size();
		picked_object = -1;
		float distance_min = 0.0f;
		for(size_t k=0; k<N; ++k)
		{
			const vec3 c = keyframe_position[k];
			const picking_info info = ray_intersect_sphere(r, c, 0.1f);

			if( info.picking_valid ) // the ray intersects a sphere
			{
				const float distance = norm(info.intersection-r.p); // get the closest intersection
				if( picked_object==-1 || distance<distance_min ){
					picked_object = k;
				}
			}
		}
	}

}

void scene_exercise::mouse_move(scene_structure& scene, GLFWwindow* window)
{
	// Mouse move is used to translate a position once selected
	// ******************************************************************** //

	// Window size
	int w=0;
	int h=0;
	glfwGetWindowSize(window, &w, &h);

	// Current cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Convert pixel coordinates to relative screen coordinates between [-1,1]
	const float x = 2*float(xpos)/float(w)-1;
	const float y = 1-2*float(ypos)/float(h);

	// Check that the mouse is clicked (drag and drop)
	const bool mouse_click_left  = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT )==GLFW_PRESS);
	const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

	const size_t N = keyframe_position.size();
	if(mouse_click_left && key_shift && picked_object!=-1)
	{
		// Translate the selected object to the new pointed mouse position within the camera plane
		// ************************************************************************************** //

		// Get vector orthogonal to camera orientation
		const mat4 M = scene.camera.camera_matrix();
		const vec3 n = {M(0,2),M(1,2),M(2,2)};

		// Compute intersection between current ray and the plane orthogonal to the view direction and passing by the selected object
		const ray r = picking_ray(scene.camera, x,y);
		vec3& p0 = keyframe_position[picked_object];
		const picking_info info = ray_intersect_plane(r,n,p0);

		// translate the position
		p0 = info.intersection;

		// Make sure that duplicated positions are moved together
		int Ns = N;
		if(picked_object==0 || picked_object==Ns-3){
			keyframe_position[0] = info.intersection;
			keyframe_position[N-3] = info.intersection;
		}
		if(picked_object==1 || picked_object==Ns-2){
			keyframe_position[1] = info.intersection;
			keyframe_position[N-2] = info.intersection;
		}
		if(picked_object==2 || picked_object==Ns-1){
			keyframe_position[2] = info.intersection;
			keyframe_position[N-1] = info.intersection;
		}

	}
}

mesh mesh_foot(float scale) {
	mesh shape;

	shape.position.resize(11);

   	shape.position[0] = {-0.1f, 0, 0};
	shape.position[1] = {-0.4f, 0, -0.4f};
	shape.position[2] = {-0.15f, 0, -0.3f};
	shape.position[3] = {0, 0, -0.5f};
	shape.position[4] = {0.15f, 0, -0.3f};
	shape.position[5] = {0.4f, 0, -0.4f};
	shape.position[6] = {0.1f, 0, 0};
	shape.position[7] = {-0.1f, 0.05f, 0};
	shape.position[8] = {-0.15f, 0.05f, -0.3f};
	shape.position[9] = {0.15f, 0.05f, -0.3f};
	shape.position[10] = {0.1f, 0.05f, 0};

	for (size_t i=0; i<shape.position.size(); i++) {
		shape.position[i] *= scale;
	}

	shape.connectivity.push_back({1, 2, 0});
	shape.connectivity.push_back({2, 3, 0});
	shape.connectivity.push_back({3, 6, 0});
	shape.connectivity.push_back({3, 4, 6});
	shape.connectivity.push_back({4, 5, 6});
	shape.connectivity.push_back({5, 9, 10});
	shape.connectivity.push_back({3, 8, 9});
	shape.connectivity.push_back({8, 1, 7});
	shape.connectivity.push_back({9, 8, 10});
	shape.connectivity.push_back({8, 7, 10});
	shape.connectivity.push_back({7, 0, 10});
	shape.connectivity.push_back({0, 6, 10});
	shape.connectivity.push_back({1, 0, 7});
	shape.connectivity.push_back({6, 5, 10});
	shape.connectivity.push_back({1, 8, 2});
	shape.connectivity.push_back({2, 8, 3});
	shape.connectivity.push_back({9, 4, 3});
	shape.connectivity.push_back({4, 9, 5});

	return shape;
}

mesh mesh_primitive_ellipsoid(float radius0, float radius1, float radius2, const vec3& p0, size_t Nu, size_t Nv)
{
	mesh shape;
	for( size_t ku=0; ku<Nu; ++ku ) {
		for( size_t kv=0; kv<Nv; ++kv ) {

			const float u = static_cast<float>(ku)/static_cast<float>(Nu-1);
			const float v = static_cast<float>(kv)/static_cast<float>(Nv);

			const float theta = static_cast<float>( 3.14159f*u );
			const float phi   = static_cast<float>( 2*3.14159f*v );

			const float x = radius0 * std::sin(theta) * std::cos(phi);
			const float y = radius1 * std::sin(theta) * std::sin(phi);
			const float z = radius2 * std::cos(theta);

			const vec3 p = {x,y,z};
			const vec3 n = normalize(p);

			shape.position.push_back( p+p0 );
			shape.normal.push_back( n );
		}
	}

	shape.connectivity = connectivity_grid(Nu, Nv, false, true);

	return shape;
}

mesh mesh_primitive_quad_scale(float scale, const vec3& p00, const vec3& p10, const vec3& p11, const vec3& p01) {
	return mesh_primitive_quad(scale*p00, scale*p10, scale*p11, scale*p01);
}

mesh create_skybox() {
    const vec3 p000 = {-1,-1, 1};
    const vec3 p001 = {-1, 1, 1};
    const vec3 p010 = {-1,-1,-1};
    const vec3 p011 = {-1, 1,-1};
    const vec3 p100 = { 1,-1, 1};
    const vec3 p101 = { 1, 1, 1};
    const vec3 p110 = { 1,-1,-1};
    const vec3 p111 = { 1, 1,-1};

    mesh skybox;

    skybox.position = {
        p000, p100, p110, p010,
        p010, p110, p111, p011,
        p100, p110, p111, p101,
        p000, p001, p010, p011,
        p001, p101, p111, p011,
        p000, p100, p101, p001
    };


    skybox.connectivity = {
        {0,1,2}, {0,2,3}, {4,5,6}, {4,6,7},
        {8,11,10}, {8,10,9}, {17,16,19}, {17,19,18},
        {23,22,21}, {23,21,20}, {13,12,14}, {13,14,15}
    };

    const float e = 1e-3f;
    const float u0 = 0.0f;
    const float u1 = 0.25f+e;
    const float u2 = 0.5f-e;
    const float u3 = 0.75f-e;
    const float u4 = 1.0f;
    const float v0 = 0.0f;
    const float v1 = 1.0f/3.0f+e;
    const float v2 = 2.0f/3.0f-e;
    const float v3 = 1.0f;
    skybox.texture_uv = {
        {u1,v1}, {u2,v1}, {u2,v2}, {u1,v2},
        {u1,v2}, {u2,v2}, {u2,v3}, {u1,v3},
        {u2,v1}, {u2,v2}, {u3,v2}, {u3,v1},
        {u1,v1}, {u0,v1}, {u1,v2}, {u0,v2},
        {u4,v1}, {u3,v1}, {u3,v2}, {u4,v2},
        {u1,v1}, {u2,v1}, {u2,v0}, {u1,v0}
    };
    return skybox;
}

void scene_exercise::update_terrain() {
	// Clear memory in case of pre-existing terrain
	terrain.data_gpu.clear();

	// Create visual terrain surface
	terrain = create_terrain(gui_scene);
	terrain.uniform_parameter.color = {1.0f, 1.0f, 1.0f};
	terrain.uniform_parameter.shading.specular = 0.0f;
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

// Evaluate 3D position of the cylinder for any (u,v) \in [0,1]
vec3 evaluate_cylinder(float u, float v, int ku, float radius, float height) {
	const float x = radius*u;
	const float y = radius*v;
	const float z = ku%2 * height;

	return {x,y,z};
}

mesh create_cylinder(float radius, float height) {
	const size_t N = 20;

	mesh cylinder; // temporary terrain storage (CPU only)
	cylinder.position.resize(2*N);

	// Fill cylinder geometry
	for(size_t ku=0; ku<2*N; ++ku) {
		// Compute local parametric coordinates (u,v) \in [0,1]
		const float u = sin(ku/2*2 /(2.0*N) * 2*M_PI );
		const float v = cos(ku/2*2 /(2.0*N) * 2*M_PI );

		// Compute coordinates
		cylinder.position[ku] = evaluate_cylinder(u,v,ku, radius, height);
	}

	// Generate triangle organization
	for(unsigned int ku=0; ku<2*N-2; ++ku)
	{
		const index3 triangle = {ku, ku+1, ku+2};
		cylinder.connectivity.push_back(triangle);
	}
	const index3 triangle2 = {2*N-2, 2*N-1, 0};
	const index3 triangle3 = {2*N-1,     0, 1};
	cylinder.connectivity.push_back(triangle2);
	cylinder.connectivity.push_back(triangle3);

	return cylinder;
}



mesh create_cone(float radius, float height, float z_offset) {
	const size_t N = 20;

	mesh cone; // temporary terrain storage (CPU only)
	cone.position.resize(N+2);

	// Fill cylinder geometry
	for(size_t ku=0; ku<N; ++ku) {
		// Compute local parametric coordinates (u,v) \in [0,1]
		const float u = sin(ku /(N+0.0) * 2*M_PI );
		const float v = cos(ku /(N+0.0) * 2*M_PI );

		// Compute coordinates
		cone.position[ku] = {radius*u, radius*v, z_offset};
	}
	cone.position[N] = {0, 0, z_offset};
	cone.position[N+1] = {0, 0, z_offset + height};

	// Generate triangle organization
	for (unsigned int ku=0; ku<N-1; ++ku) {
		const index3 triangle = {(ku+1), ku, N};
		const index3 triangle2 = {(ku+1), ku, N+1};
		cone.connectivity.push_back(triangle);
		cone.connectivity.push_back(triangle2);
	}
	const index3 triangle = {0, N-1, N};
	const index3 triangle2 = {0, N-1, N+1};
	cone.connectivity.push_back(triangle);
	cone.connectivity.push_back(triangle2);

	return cone;
}

mesh create_tree_foliage(float radius, float height, float z_offset) {
	mesh m = create_cone(radius, height, 0);
	m.push_back( create_cone(radius, height, z_offset) );
	m.push_back( create_cone(radius, height, 2*z_offset) );

	return m;
}


void scene_exercise::update_tree_position() {
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
		tree_position.push_back(evaluate_terrain(u,v, gui_scene));
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

void scene_exercise::update_mushroom_position() {
	unsigned seed = 2;
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator(seed);
	size_t number_of_mushrooms = 40;

	for (size_t i=0; i<number_of_mushrooms; i++) {
		float u = distrib(generator);
		float v = distrib(generator);
		mushroom_position.push_back(evaluate_terrain(u,v, gui_scene));
	}
}

void scene_exercise::update_bill_grass_position() {
	unsigned seed = 3;
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator(seed);
	size_t number_of_grass = 40;

	for (size_t i=0; i<number_of_grass; i++) {
		float u = distrib(generator);
		float v = distrib(generator);
		grass_position.push_back(evaluate_terrain(u,v, gui_scene));
	}
}

void scene_exercise::update_bill_flower_position() {
	unsigned seed = 4;
	std::uniform_real_distribution<float> distrib(0.0,1.0);
	std::default_random_engine generator(seed);
	size_t number_of_flowers = 40;

	for (size_t i=0; i<number_of_flowers; i++) {
		float u = distrib(generator);
		float v = distrib(generator);
		flower_position.push_back(evaluate_terrain(u,v, gui_scene));
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
	ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);

	const float timek_scale_min = 0.1f;
	const float timek_scale_max = 3.0f;
	ImGui::SliderFloat("Time scale", &timer.scale, timek_scale_min, timek_scale_max);

	if( ImGui::Button("Print Keyframe") )
	{
		std::cout<<"keyframe_position={";
		for(size_t k=0; k<keyframe_position.size(); ++k)
		{
			const vec3& p = keyframe_position[k];
			std::cout<< "{"<<p.x<<"f,"<<p.y<<"f,"<<p.z<<"f}";
			if(k<keyframe_position.size()-1)
				std::cout<<", ";
		}
		std::cout<<"}"<<std::endl;
	}


	ImGui::SliderFloat("Penguin Time", &penguin_timer.t, penguin_timer.t_min, penguin_timer.t_max);

	const float penguin_time_scale_min = 0.1f;
	const float penguin_time_scale_max = 3.0f;
	ImGui::SliderFloat("Penguin Time scale", &penguin_timer.scale, penguin_time_scale_min, penguin_time_scale_max);

	ImGui::Checkbox("Trajectory", &gui_scene.trajectory);
}

static size_t index_at_value(float t, const std::vector<float>& vt)
{
	const size_t N = vt.size();
	assert(vt.size()>=2);
	assert(t>=vt[0]);
	assert(t<vt[N-1]);

	size_t k=0;
	while( vt[k+1]<t )
		++k;
	return k;
}

static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3) {
	const float s = (t-t1)/(t2-t1);

	const float mu = 0.5;

	const vec3 d1 = mu * ((t3-t1)/(t2-t0)) * (p2-p0);
	const vec3 d2 = mu * ((t2-t0)/(t3-t1)) * (p3-p1);

	const vec3 p = (2*s*s*s - 3*s*s + 1)*p1 + (s*s*s - 2*s*s + s)*d1 + (-2*s*s*s + 3*s*s)*p2 + (s*s*s - s*s)*d2;

	return p;
}


static vec3 cardinal_spline_interpolation_derivative(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3) {
	const float s = (t-t1)/(t2-t1);
	const float dsdt = 1.0f/(t2-t1);

	const float mu = 0.5;

	const vec3 d1 = mu * ((t3-t1)/(t2-t0)) * (p2-p0);
	const vec3 d2 = mu * ((t2-t0)/(t3-t1)) * (p3-p1);

	const vec3 d = (6*s*s*dsdt - 6*s*dsdt)*p1 + (3*s*s*dsdt - 4*s*dsdt + dsdt)*d1 + (-6*s*s*dsdt + 6*s*dsdt)*p2 + (3*s*s*dsdt - 2*s*dsdt)*d2;

	return d;
}

static vec3 cardinal_spline_interpolation(float t, std::vector<vcl::vec3> keyframe_position, std::vector<float> keyframe_time) {
	const size_t idx = index_at_value(t, keyframe_time);
	// Linear interpolation
	/*
	const float t1 = keyframe_time[idx];
	const float t2 = keyframe_time[idx+1];

	const vec3& p1 = keyframe_position[idx];
	const vec3& p2 = keyframe_position[idx+1];


	const vec3 p = linear_interpolation(t,t1,t2,p1,p2);
	trajectory.add_point(p);
	*/

	// Cardianl Spline interpolation
	const float t0 = keyframe_time[idx-1];
	const float t1 = keyframe_time[idx];
	const float t2 = keyframe_time[idx+1];
	const float t3 = keyframe_time[idx+2];

	const vec3& p0 = keyframe_position[idx-1];
	const vec3& p1 = keyframe_position[idx];
	const vec3& p2 = keyframe_position[idx+1];
	const vec3& p3 = keyframe_position[idx+2];
	const vec3 p = cardinal_spline_interpolation(t,t0,t1,t2,t3,p0,p1,p2,p3);

	return p;
}

static vec3 cardinal_spline_interpolation_derivative(float t, std::vector<vcl::vec3> keyframe_position, std::vector<float> keyframe_time) {
	const size_t idx = index_at_value(t, keyframe_time);

	const float t0 = keyframe_time[idx-1];
	const float t1 = keyframe_time[idx];
	const float t2 = keyframe_time[idx+1];
	const float t3 = keyframe_time[idx+2];

	const vec3& p0 = keyframe_position[idx-1];
	const vec3& p1 = keyframe_position[idx];
	const vec3& p2 = keyframe_position[idx+1];
	const vec3& p3 = keyframe_position[idx+2];
	const vec3 p = cardinal_spline_interpolation_derivative(t,t0,t1,t2,t3,p0,p1,p2,p3);

	return p;
}




#endif
