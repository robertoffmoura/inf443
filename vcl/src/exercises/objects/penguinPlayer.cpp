#include "penguinPlayer.hpp"
#include "terrain.hpp"
#include <iostream>

using namespace vcl;

mesh mesh_foot(float scale);
mesh mesh_primitive_quad_scale(float scale, const vec3& p00, const vec3& p10, const vec3& p11, const vec3& p01);
mesh mesh_primitive_ellipsoid(float radius0, float radius1, float radius2, const vec3& p0, size_t Nu, size_t Nv);
mesh mesh_primitive_prism(float scale, float height, const vec3& p00, const vec3& p10, const vec3& p11, const vec3& p01);


void PenguinPlayer::setup(float scale) {
	x = 3;
	y = 4;
	angle = 0;
	camera_angle = 0;
	//Penguin::setup(scale);
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

	/*
	mesh_drawable arm_right = mesh_primitive_quad_scale(arm, {0.2f,0.25f,0}, {0.4f,0.2f,0}, {0.4f,0.025f,0}, {0.2f,0.025f,0});
	mesh_drawable arm_lower_right = mesh_primitive_quad_scale(arm, {0,0.2f,0}, {0.2f,0.1f,0}, {0.2f,0,0}, {0,0.025f,0});
	mesh_drawable hand_right = mesh_primitive_quad_scale(arm, {0,0.1f,0}, {0.03f,0.07f,0}, {0.03f,0.03f,0}, {0,0,0});

	mesh_drawable arm_left = mesh_primitive_quad_scale(arm, {-0.2f,0.25f,0}, {-0.4f,0.2f,0}, {-0.4f,0.025f,0}, {-0.2f,0.025f,0});
	mesh_drawable arm_lower_left = mesh_primitive_quad_scale(arm, {-0,0.2f,0}, {-0.2f,0.1f,0}, {-0.2f,0,0}, {-0,0.025f,0});
	mesh_drawable hand_left = mesh_primitive_quad_scale(arm, {-0,0.1f,0}, {-0.03f,0.07f,0}, {-0.03f,0.03f,0}, {0,0,0});
	*/
	
	mesh_drawable arm_right = mesh_primitive_prism(arm, arm/60, {0,0.1125f,0}, {0.2f,0.0625f,0}, {0.2f,-0.1125f,0}, {0,-0.1125f,0});
	mesh_drawable arm_lower_right = mesh_primitive_prism(arm, arm/60, {0,0.2f,0}, {0.2f,0.1f,0}, {0.2f,0,0}, {0,0.025f,0});
	mesh_drawable hand_right = mesh_primitive_prism(arm, arm/60, {0,0.1f,0}, {0.03f,0.07f,0}, {0.03f,0.03f,0}, {0,0,0});

	mesh_drawable arm_left = mesh_primitive_prism(arm, -arm/60, {0,0.1125f,0}, {-0.2f,0.0625f,0}, {-0.2f,-0.1125f,0}, {0,-0.1125f,0});
	mesh_drawable arm_lower_left = mesh_primitive_prism(arm, -arm/60, {-0,0.2f,0}, {-0.2f,0.1f,0}, {-0.2f,0,0}, {-0,0.025f,0});
	mesh_drawable hand_left = mesh_primitive_prism(arm, -arm/60, {-0,0.1f,0}, {-0.03f,0.07f,0}, {-0.03f,0.03f,0}, {0,0,0});
	
	//mesh_drawable test = mesh_primit
	
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
	hierarchy.add_element(foot, "foot_left", "body", {-0.625f*r_body0,-0.8f*r_body1,0}, rotation_from_axis_angle_mat3({0,1,0}, 0.3f));
	hierarchy.add_element(foot, "foot_right", "body", {0.625f*r_body0,-0.8f*r_body1,0}, rotation_from_axis_angle_mat3({0,1,0}, -0.3f));

	hierarchy.add_element(eye, "eye_left", "head",{r_head/3,r_head/1.5f,r_head/2});
	hierarchy.add_element(eye, "eye_right", "head",{-r_head/3,r_head/1.5f,r_head/2});
	hierarchy.add_element(pupil, "pupil_left", "head",{1.2f*r_head/3, 1.2f*r_head/1.5f, 1.2f*r_head/2});
	hierarchy.add_element(pupil, "pupil_right", "head",{-1.2f*r_head/3, 1.2f*r_head/1.5f, 1.2f*r_head/2});
	hierarchy.add_element(beak, "beak", "head", {0,r_head/1.5f,0});

	hierarchy.add_element(arm_right, "arm_right", "body", {0.2f*arm, 0.175f*r_body1, 0});
	hierarchy.add_element(arm_lower_right, "arm_lower_right", "arm_right", {0.2f*arm, -0.1375f*arm,0});
	hierarchy.add_element(hand_right, "hand_right", "arm_lower_right", {0.2f*arm,0,0});

	hierarchy.add_element(arm_left, "arm_left", "body", {-0.2f*arm, 0.175f*r_body1, 0});
	hierarchy.add_element(arm_lower_left, "arm_lower_left", "arm_left", {-0.2f*arm, -0.1375f*arm,0});
	hierarchy.add_element(hand_left, "hand_left", "arm_lower_left", {-0.2f*arm,0,0});

	penguin_timer.scale = 0.8f;
}

void PenguinPlayer::key_press(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods) {
	moving_up = (glfwGetKey(window, GLFW_KEY_W));
	moving_down = (glfwGetKey(window, GLFW_KEY_S));
	moving_left = (glfwGetKey(window, GLFW_KEY_A));
	moving_right = (glfwGetKey(window, GLFW_KEY_D));
}

void PenguinPlayer::updatePosition(scene_structure& scene) {
	moving = moving_up || moving_down || moving_left || moving_right;
	float speed = 0.05f;
	if (moving_up) {
		angle = 0;
	} else if (moving_down) {
		angle = M_PI;
	}

	if (moving_left) {
		//move(0, 0.1f);
		//turn(-0.02f);
		angle = - M_PI/2.0f;
	} else if (moving_right) {
		//move(0, -0.1f);
		//turn(0.02f);
		angle = M_PI/2.0f;
	}

	if (moving_left && moving_up) {
		angle = - M_PI/4.0f;
	} else if (moving_right && moving_up) {
		angle = M_PI/4.0f;
	} else if (moving_left && moving_down) {
		angle = M_PI + M_PI/4.0f;
	} else if (moving_right && moving_down) {
		angle = M_PI - M_PI/4.0f;
	}

	if (moving_up || moving_down || moving_left || moving_right) {
		move(speed*sin(camera_angle + angle), speed*cos(camera_angle + angle));
	}

	if (scene.camera_control.moving_left) {
		camera_angle -= 0.02f;
		scene.camera.apply_rotation(0.02f, 0, 0, 0);
	} else if (scene.camera_control.moving_right) {
		camera_angle += 0.02f;
		scene.camera.apply_rotation(-0.02f, 0, 0, 0);
	}

	if (scene.camera_control.moving_camera_up) {
		scene.camera.apply_rotation(0, 0.01f, 0, 0);
	} else if (scene.camera_control.moving_camera_down) {
		scene.camera.apply_rotation(0, -0.01f, 0, 0);
	}

	if (scene.camera_control.moving_camera_left) {
		camera_angle -= 0.05f;
		angle += 0.05f;
		scene.camera.apply_rotation(0.05f, 0, 0, 0);
	} else if (scene.camera_control.moving_camera_right) {
		camera_angle += 0.05f;
		angle -= 0.05f;
		scene.camera.apply_rotation(-0.05f, 0, 0, 0);
	}
}

void PenguinPlayer::move(float dx, float dy) {
	x += dx;
	y += dy;
}

void PenguinPlayer::turn(float d_angle) {
	angle += d_angle;
}

void PenguinPlayer::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe, vec3 position, vec3 derivative, Terrain& terrain) {
	updatePosition(scene);
	penguin_timer.update();
	float z = terrain.evaluate_terrain_z_real(x, y);
	const vec3 p = {x, y, z};
	//std::cout << p << std::endl;
	const vec3 height = {0, 0, 0.45f};
	const vec3 headHeight = {0, 0, 0.3f};
	
	hierarchy.translation("body") = p + position + height;
	scene.camera.translation = -hierarchy.translation("body") - headHeight;
	
	if (moving) {
	//set_gui();
	
	const float pt = penguin_timer.t;

	hierarchy.translation("body") += {0,0,0.04f*(sin(8*3.14f*pt))};
	/*
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
	*/
	//const vec3 rotationFix = {0.1f*(1-cos(3.1415f/4)), 0.1f*(sin(3.1415f/4)), 0};
	//const vec3 rotationFix2 = {-0.1f*(1-cos(3.1415f/4)), 0.1f*(sin(3.1415f/4)), 0};
	hierarchy.rotation("arm_left") = rotation_from_axis_angle_mat3({1,0,0}, 1.45f*sin(2*3.14f*(2*pt-0.4f)) )*rotation_from_axis_angle_mat3({0,0,1}, M_PI/4)*rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	hierarchy.rotation("arm_lower_left") = rotation_from_axis_angle_mat3({0,1,0}, -M_PI/8);
	hierarchy.rotation("hand_left") = rotation_from_axis_angle_mat3({0,1,0}, -M_PI/8);
	hierarchy.translation("arm_left") = {-0.12f, 0.105f, 0}; 

	hierarchy.rotation("arm_right") = rotation_from_axis_angle_mat3({1,0,0}, -1.45f*sin(2*3.14f*(2*pt-0.4f)) )*rotation_from_axis_angle_mat3({0,0,1}, -M_PI/4)*rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	hierarchy.rotation("arm_lower_right") = rotation_from_axis_angle_mat3({0,1,0}, M_PI/8);
	hierarchy.rotation("hand_right") = rotation_from_axis_angle_mat3({0,1,0}, M_PI/8);
	hierarchy.translation("arm_right") = {0.12f, 0.105f, 0}; 
	//hierarchy.translation("arm_right") = vec3(0, 0.15f, 0.068f); 

	/*
	hierarchy.translation("arm_left") = {0,0.125f,0.07f};
	hierarchy.rotation("arm_left") = rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	hierarchy.translation("arm_right") = {0,0.125f,0.07f}; //without rotation
	hierarchy.rotation("arm_right") = rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2); //without rotation
	*/
	
	hierarchy.rotation("foot_left") =  rotation_from_axis_angle_mat3({1,0,0}, -3.14/4 + 1.55f*sin(4*3.14f*(pt-0.4f)))*rotation_from_axis_angle_mat3({0,1,0}, 0.3f);
	hierarchy.rotation("foot_right") = rotation_from_axis_angle_mat3({1,0,0}, -3.14/4 - 1.55f*sin(4*3.14f*(pt-0.4f)))*rotation_from_axis_angle_mat3({0,1,0}, -0.3f);


	hierarchy.rotation("body") = rotation_from_axis_angle_mat3({0,0,-1}, camera_angle + angle)*rotation_from_axis_angle_mat3({1,0,0}, M_PI/2);
	hierarchy.rotation("neck") = rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	} else {
	penguin_timer.t = 0;
	//scene.camera_control.
	//const vec3 d = derivative;
	//hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2)*rotation_between_vector_mat3({0,0,1}, d);
	//hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2)*rotation_between_vector_mat3({0,1,0}, d);
	//hierarchy.rotation("body") = rotation_between_vector_mat3({0,1,0}, d);

	//const vec3 rotationFix = {0.1f*(1-cos(3.1415f/4)), 0.1f*(sin(3.1415f/4)), 0};
	//const vec3 rotationFix2 = {-0.1f*(1-cos(3.1415f/4)), 0.1f*(sin(3.1415f/4)), 0};
	//hierarchy.rotation("arm_left") = rotation_from_axis_angle_mat3({0,0,1}, M_PI/4)*rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	//hierarchy.rotation("arm_lower_left") = rotation_from_axis_angle_mat3({0,1,0}, -M_PI/8);
	//hierarchy.rotation("hand_left") = rotation_from_axis_angle_mat3({0,1,0}, -M_PI/8);
	//hierarchy.translation("arm_left") = vec3(0, 0.15f, 0.068f); 

	//hierarchy.rotation("arm_right") = rotation_from_axis_angle_mat3({0,0,1}, -M_PI/4)*rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	//hierarchy.rotation("arm_lower_right") = rotation_from_axis_angle_mat3({0,1,0}, M_PI/8);
	//hierarchy.rotation("hand_right") = rotation_from_axis_angle_mat3({0,1,0}, M_PI/8);
	//hierarchy.translation("arm_right") = vec3(0, 0.15f, 0.068f); 

	/*
	//hierarchy.translation("arm_left") = {0,0.125f,0.07f};
	//hierarchy.rotation("arm_left") = rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	//hierarchy.translation("arm_right") = {0,0.125f,0.07f}; //without rotation
	//hierarchy.rotation("arm_right") = rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2); //without rotation
	*/
	hierarchy.rotation("arm_left") = rotation_from_axis_angle_mat3({0,0,1}, M_PI/4)*rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	hierarchy.rotation("arm_right") = rotation_from_axis_angle_mat3({0,0,1}, -M_PI/4)*rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);

	hierarchy.rotation("foot_left") =  rotation_from_axis_angle_mat3({0,1,0}, 0.3f);
	hierarchy.rotation("foot_right") = rotation_from_axis_angle_mat3({0,1,0}, -0.3f);

	hierarchy.rotation("body") = rotation_from_axis_angle_mat3({0,0,-1}, camera_angle + angle)*rotation_from_axis_angle_mat3({1,0,0}, M_PI/2);
	hierarchy.rotation("neck") = rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	//std::cout << d << std::endl;
	}

	hierarchy.draw(shaders["mesh"], scene.camera);
	if (wireframe) {
		hierarchy.draw(shaders["wireframe"], scene.camera);
	}
}
