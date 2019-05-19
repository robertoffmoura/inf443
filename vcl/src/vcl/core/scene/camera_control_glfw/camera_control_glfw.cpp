#include "camera_control_glfw.hpp"


namespace vcl
{


enum class camera_action_state {none, translation, rotation, scale, translation_depth};

void camera_control_glfw::update_mouse_move(camera_scene& camera, GLFWwindow* window, float x1, float y1)
{
    assert(window!=nullptr);

    // Get type of click (left, right), is CTRL pressed
    const bool mouse_click_left  = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT )==GLFW_PRESS);
    const bool mouse_click_right = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS);
    const bool key_ctrl = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL));
    const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

    if(update==false || key_shift)
    {
        x0 = x1;
        y0 = y1;
        return;
    }




    // Get window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);

    // Set action state accordingly
    camera_action_state state = camera_action_state::none;

    if(!mouse_click_left && !mouse_click_right)
        state = camera_action_state::none;
    else if( mouse_click_left && !key_ctrl )
        state = camera_action_state::rotation;
    else if( mouse_click_left && key_ctrl )
        state = camera_action_state::translation;
    else if( mouse_click_right && !key_ctrl )
        state = camera_action_state::scale;
    else if( mouse_click_right && key_ctrl )
        state = camera_action_state::translation_depth;


    // ************************************************* //
    // Compute transformation to apply on the camera
    // ************************************************* //
    if( state == camera_action_state::translation ) {

        const float tx =  (x1-x0)/w;
        const float ty = -(y1-y0)/h;
        camera.apply_translation_in_screen_plane( tx, ty );

    }

    else if( state == camera_action_state::rotation ) {

        // relative position on screen
        const float ux0 = 2*x0/float(w)-1;
        const float uy0 = 1-2*y0/float(h);

        const float ux1 = 2*x1/float(w)-1;
        const float uy1 = 1-2*y1/float(h);

        // apply rotation
        camera.apply_rotation(ux0, uy0, ux1, uy1);
    }

    else if ( state == camera_action_state::scale ) {

        const float scale_magnitude = (y0-y1)/h;

        camera.apply_scaling( scale_magnitude );
    }

    else if ( state == camera_action_state::translation_depth ) {
        const float translation_magnitude = (y1-y0)/h;

        camera.apply_translation_orthogonal_to_screen_plane( translation_magnitude );
    }


    // Update previous click position
    x0 = x1;
    y0 = y1;

}


void camera_control_glfw::update_mouse_click(camera_scene& , GLFWwindow* , int , int , int )
{

}

void camera_control_glfw::update_key_press(camera_scene& camera, GLFWwindow* window,  int key, int scancode, int action, int mods) {
	//const bool key_W = (glfwGetKey(window, GLFW_KEY_W));
	//std::cout << key_W << std::endl;
	/*
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
        //std::cout << "E Press" << std::endl;
		moving_up = true;
	*/
	moving_up = (glfwGetKey(window, GLFW_KEY_W));
	moving_down = (glfwGetKey(window, GLFW_KEY_S));
	moving_left = (glfwGetKey(window, GLFW_KEY_A));
	moving_right = (glfwGetKey(window, GLFW_KEY_D));

	moving_camera_up = (glfwGetKey(window, GLFW_KEY_UP));
	moving_camera_down = (glfwGetKey(window, GLFW_KEY_DOWN));
	moving_camera_left = (glfwGetKey(window, GLFW_KEY_LEFT));
	moving_camera_right = (glfwGetKey(window, GLFW_KEY_RIGHT));
}

void camera_control_glfw::update_keyframe(camera_scene& camera) {
	/*if (moving_up) {
		//std::cout << "up" << std::endl;
		//camera.apply_translation_in_screen_plane(0, 0.01f);
		camera.apply_translation_orthogonal_to_screen_plane( 0.1f );
		//camera.apply_scaling(-0.01f);
	}
	else if (moving_down) {
		//std::cout << "down" << std::endl;
		//camera.apply_translation_in_screen_plane(0, -0.01f);
		camera.apply_translation_orthogonal_to_screen_plane( -0.1f );
		//camera.apply_scaling(0.01f);
	}
	if (moving_left) {
		//std::cout << "left" << std::endl;
		camera.apply_translation_in_screen_plane(0.1f, 0);
	}
	else if (moving_right) {
		//std::cout << "right" << std::endl;
		camera.apply_translation_in_screen_plane(-0.1f, 0);
	}
	if (moving_camera_up) {
		camera.apply_rotation(0, -0.01f, 0, 0);
	} else if (moving_camera_down) {
		camera.apply_rotation(0, 0.01f, 0, 0);
	}

	if (moving_camera_left) {
		camera.apply_rotation(0.02f, 0, 0, 0);
	} else if (moving_camera_right) {
		camera.apply_rotation(-0.02f, 0, 0, 0);
	}
	*/
	/*
	if (moving_left) {
		camera.apply_rotation(0.02f, 0, 0, 0);
	} else if (moving_right) {
		camera.apply_rotation(-0.02f, 0, 0, 0);
	}

	if (moving_camera_up) {
		camera.apply_rotation(0, 0.01f, 0, 0);
	} else if (moving_camera_down) {
		camera.apply_rotation(0, -0.01f, 0, 0);
	}

	if (moving_camera_left) {
		camera.apply_rotation(0.05f, 0, 0, 0);
	} else if (moving_camera_right) {
		camera.apply_rotation(-0.05f, 0, 0, 0);
	}
	*/
}

}
