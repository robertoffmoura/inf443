#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
//#include "../../vcl/core/math/vec/vec.hpp"
#include <vector>
#include "../objects/palm_tree.hpp"
#include "../objects/skybox.hpp"
#include "../objects/penguin.hpp"
#include "../objects/trajectory.hpp"
#include "../objects/tree.hpp"

#ifdef INF443_01_PROJECT
// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe;

    float height = 0.2f;
    float scaling = 6.0f;
    int octave = 7;
    float persistency = 0.5f;

	bool trajectory = false;
};

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

    void setup_terrain();
    void setup_mushroom();
    void setup_flower();
    void setup_grass();

    void draw_terrain(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_mushroom(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_grass(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_flower(std::map<std::string,GLuint>& shaders, scene_structure& scene);

    void set_tree_position();
    void update_mushroom_position();
    void update_bill_grass_position();
    void update_bill_flower_position();
    void update_terrain();

    GLuint terrain_texture_id;
    GLuint bill_grass_texture_id;
    GLuint bill_flower_texture_id;
    std::vector<vcl::vec3> mushroom_position;
    std::vector<vcl::vec3> grass_position;
    std::vector<vcl::vec3> flower_position;
    // visual representation of a surface
    vcl::mesh_drawable terrain;

	Tree tree;

    vcl::mesh_drawable mushroom_cylinder;
    vcl::mesh_drawable mushroom_cone;

    vcl::mesh_drawable grass_surface;
    vcl::mesh_drawable flower_surface;
	Skybox skybox;

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
