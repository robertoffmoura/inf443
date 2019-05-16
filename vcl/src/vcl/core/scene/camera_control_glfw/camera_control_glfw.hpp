#pragma once

#include "../camera/camera.hpp"
#include <GLFW/glfw3.h>


namespace vcl
{

class camera_control_glfw
{
public:

    void update_mouse_move(camera_scene& camera, GLFWwindow* window, float x1, float y1);
    void update_mouse_click(camera_scene& camera, GLFWwindow* window,  int button, int action, int mods);
	void update_key_press(camera_scene& camera, GLFWwindow* window,  int key, int scancode, int action, int mods);
	void update_keyframe(camera_scene& camera);
    bool update = true;

private:

    /** previous mouse position (x-coordinate) */
    float x0 = 0.0f;
    /** previous mouse position (y-coordinate) */
    float y0 = 0.0f;

	bool moving_up;
	bool moving_down;
	bool moving_left;
	bool moving_right;

	bool moving_camera_up;
	bool moving_camera_down;
	bool moving_camera_left;
	bool moving_camera_right;

};

}
