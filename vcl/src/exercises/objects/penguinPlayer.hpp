#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "penguin.hpp"
#include <iostream>

class Terrain;

class PenguinPlayer : Penguin {
public:
    void setup(float scale);
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe, vcl::vec3 position, vcl::vec3 derivative, Terrain& terrain);
	void key_press(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);
	void updatePosition(scene_structure& scene);
	void move(float dx, float dy);
	void turn(float d_angle);
	float angle;
	float camera_angle;
	float x;
	float y;
	//vcl::timer_interval penguin_timer;
	bool moving_up;
	bool moving_down;
	bool moving_left;
	bool moving_right;
	bool moving;

private:
	//vcl::mesh_drawable_hierarchy hierarchy;

};