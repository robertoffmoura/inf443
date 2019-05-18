#include "penguinPlayer.hpp"
#include "terrain.hpp"
#include <iostream>

using namespace vcl;

void PenguinPlayer::setup(float scale) {
	x = 3;
	y = 4;
	Penguin::setup(scale);
}

void PenguinPlayer::key_press(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods) {
	moving_up = (glfwGetKey(window, GLFW_KEY_W));
	moving_down = (glfwGetKey(window, GLFW_KEY_S));
	moving_left = (glfwGetKey(window, GLFW_KEY_A));
	moving_right = (glfwGetKey(window, GLFW_KEY_D));
}

void PenguinPlayer::updatePosition() {
	float speed = 0.05f;
	if (moving_up) {
		move(speed*sin(angle), speed*cos(angle));
	} else if (moving_down) {
		move(-speed*sin(angle), -speed*(cos(angle)));
	}

	if (moving_left) {
		//move(0, 0.1f);
		turn(-0.02f);
	} else if (moving_right) {
		//move(0, -0.1f);
		turn(0.02f);
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
	updatePosition();
	penguin_timer.update();
	//set_gui();
	/*
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
	*/
	float z = terrain.evaluate_terrain_z_real(x, y);
	const vec3 p = {x, y, z};
	std::cout << p << std::endl;
	const vec3 height = {0, 0, 0.45f};
	
	hierarchy.translation("body") = p + position + height;
	scene.camera.translation = -hierarchy.translation("body");
	//const vec3 d = derivative;
	//hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2)*rotation_between_vector_mat3({0,0,1}, d);
	//hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2)*rotation_between_vector_mat3({0,1,0}, d);
	//hierarchy.rotation("body") = rotation_between_vector_mat3({0,1,0}, d);
	hierarchy.rotation("body") = rotation_from_axis_angle_mat3({0,0,1}, -angle)*rotation_from_axis_angle_mat3({1,0,0}, M_PI/2);
	hierarchy.rotation("neck") = rotation_from_axis_angle_mat3({1,0,0}, -M_PI/2);
	//std::cout << d << std::endl;

	hierarchy.draw(shaders["mesh"], scene.camera);
	if (wireframe) {
		hierarchy.draw(shaders["wireframe"], scene.camera);
	}
}
