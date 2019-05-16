#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"

class Trajectory {
public:
    void setup();
	void draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool show);

	// Called every time the mouse is clicked
    void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    // Called every time the mouse is moved
    void mouse_move(scene_structure& scene, GLFWwindow* window);

	vcl::vec3 position();
	vcl::vec3 position_derivative();


	// Data (p_i,t_i)
    std::vector<vcl::vec3> keyframe_position; // Given positions
    std::vector<float> keyframe_time;         // Time at given positions

	//vcl::mesh_drawable surface;                            // moving point
    vcl::mesh_drawable sphere;                             // keyframe samples
    vcl::segment_drawable_immediate_mode segment_drawer;   // used to draw segments between keyframe samples
    vcl::curve_dynamic_drawable trajectory;                // Draw the trajectory of the moving point as a curve

	// Store the index of a selected sphere
    int picked_object;
    vcl::timer_interval timer;

private:

};