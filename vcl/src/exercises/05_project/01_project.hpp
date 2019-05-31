#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
//#include "../../vcl/core/math/vec/vec.hpp"
#include <vector>
#include "../objects/palm_tree.hpp"
#include "../objects/skybox.hpp"
#include "../objects/penguin.hpp"
#include "../objects/trajectory.hpp"
#include "../objects/tree.hpp"
#include "../objects/mushroom.hpp"
#include "../objects/grass.hpp"
#include "../objects/flower.hpp"
#include "../objects/terrain.hpp"
#include "../objects/gui.hpp"
#include "../objects/sea.hpp"

#ifdef INF443_01_PROJECT
// Stores some parameters that can be set from the GUI

struct scene_exercise : base_scene_exercise
{

    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */


    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    // visual representation of a surface
    Terrain terrain;

	//Tree tree;
	//Mushroom mushroom;
	//Grass grass;
    //Flower flower;
	Skybox skybox;
	Sea sea;

	/*
	vcl::mesh_drawable trunk;
	GLuint will_texture_id;
	*/


    // Called every time the mouse is clicked
    void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    // Called every time the mouse is moved
    void mouse_move(scene_structure& scene, GLFWwindow* window);

	Trajectory trajectory;
	Penguin penguin;
	Palm_tree palm_tree;

    gui_scene_structure gui_scene;
};

#endif
