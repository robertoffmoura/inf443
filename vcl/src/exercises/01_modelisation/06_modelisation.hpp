#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
//#include "../../vcl/core/math/vec/vec.hpp"
#include <vector>

#ifdef INF443_06_MODELISATION

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe;

    float height = 0.2f;
    float scaling = 6.0f;
    int octave = 7;
    float persistency = 0.5f;
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
    void setup_tree();
    void setup_mushroom();
    void setup_flower();
    void setup_sky();
    void setup_grass();

    void draw_terrain(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_tree(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_mushroom(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_grass(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_flower(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_sky(std::map<std::string,GLuint>& shaders, scene_structure& scene);

    void update_tree_position();
    void update_mushroom_position();
    void update_bill_grass_position();
    void update_bill_flower_position();
    void update_terrain();

    GLuint terrain_texture_id;
    GLuint bill_grass_texture_id;
    GLuint bill_flower_texture_id;
    GLuint sky_texture_id;

    std::vector<vcl::vec3> tree_position;
    std::vector<vcl::vec3> mushroom_position;
    std::vector<vcl::vec3> grass_position;
    std::vector<vcl::vec3> flower_position;
    // visual representation of a surface
    vcl::mesh_drawable terrain;

    vcl::mesh_drawable cone;

    vcl::mesh_drawable cylinder; // tree
    vcl::mesh_drawable foliage; // tree

    vcl::mesh_drawable mushroom_cylinder;
    vcl::mesh_drawable mushroom_cone;

    vcl::mesh_drawable grass_surface;
    vcl::mesh_drawable flower_surface;
    vcl::mesh_drawable sky_surface;

    gui_scene_structure gui_scene;
};

#endif


