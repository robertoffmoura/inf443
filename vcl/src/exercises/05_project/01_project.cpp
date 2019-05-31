
#include "01_project.hpp"

#ifdef INF443_01_PROJECT

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
using std::vector;
#include <random>
#include <iostream>


/** This function is called before the beginning of the animation loop
	It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& ) {
	trajectory.setup();
	penguin.setup(0.5f);

	//tree.set_tree_position(gui_scene, terrain);
	//mushroom.set_mushroom_position(gui_scene, terrain);
	//grass.set_bill_grass_position(gui_scene, terrain);
	//flower.set_bill_flower_position(gui_scene, terrain);
	palm_tree.set_tree_position(gui_scene, terrain);


	terrain.setup(gui_scene);
	//tree.setup();
	//mushroom.setup();
	//grass.setup();
	//flower.setup();
	skybox.setup();
	palm_tree.setup();
	sea.setup();

	// Setup initial camera mode and position
	scene.camera.camera_type = camera_control_spherical_coordinates;
	scene.camera.scale = 0.01f;
	scene.camera.apply_rotation(0,0,0,1.2f);
	scene.camera.apply_translation_orthogonal_to_screen_plane(-32.0f);

	/*
	trunk = create_cone_trunk(3.2f, 4.0f, 9.0f, {0, 0, 4.0f});
	will_texture_id = texture_gpu( image_load_png("data/will.png") );
	*/
}


/** This function is called at each frame of the animation loop.
	It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure&) {
	//trajectory.timer.update();
	set_gui();

	glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe
	glPolygonOffset( 1.0, 1.0 );

	/*
	glBindTexture(GL_TEXTURE_2D, will_texture_id);
	trunk.draw(shaders["mesh"], scene.camera);
	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
	if (gui_scene.wireframe) { // wireframe if asked from the GUI
		glPolygonOffset( 1.0, 1.0 );
		trunk.draw(shaders["wireframe"], scene.camera);
		//sand_terrain.draw(shaders["wireframe"], scene.camera);
	}
	*/


	terrain.draw(shaders, scene, gui_scene.wireframe);
	//tree.draw(shaders, scene, gui_scene.wireframe);
	//mushroom.draw(shaders, scene, gui_scene.wireframe);
	palm_tree.draw(shaders, scene, gui_scene.wireframe);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);
	skybox.draw(shaders, scene, gui_scene.wireframe);
	//sea.draw(shaders, scene, gui_scene.wireframe);
	//flower.draw(shaders, scene, gui_scene.wireframe);
	//grass.draw(shaders, scene, gui_scene.wireframe);
	sea.draw(shaders, scene, gui_scene.wireframe);
	glDepthMask(true);

	trajectory.draw(shaders, scene, gui_scene.trajectory);
	penguin.draw(shaders, scene, gui_scene.wireframe, trajectory.position(), trajectory.position_derivative());
}

void scene_exercise::mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods) {
	trajectory.mouse_click(scene, window, button, action, mods);
}

void scene_exercise::mouse_move(scene_structure& scene, GLFWwindow* window) {
	trajectory.mouse_move(scene, window);
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
	ImGui::SliderFloat("Time", &trajectory.timer.t, trajectory.timer.t_min, trajectory.timer.t_max);

	const float timek_scale_min = 0.1f;
	const float timek_scale_max = 3.0f;
	ImGui::SliderFloat("Time scale", &trajectory.timer.scale, timek_scale_min, timek_scale_max);

	if( ImGui::Button("Print Keyframe") )
	{
		std::cout<<"keyframe_position={";
		for(size_t k=0; k<trajectory.keyframe_position.size(); ++k)
		{
			const vec3& p = trajectory.keyframe_position[k];
			std::cout<< "{"<<p.x<<"f,"<<p.y<<"f,"<<p.z<<"f}";
			if(k<trajectory.keyframe_position.size()-1)
				std::cout<<", ";
		}
		std::cout<<"}"<<std::endl;
	}


	ImGui::SliderFloat("Penguin Time", &penguin.penguin_timer.t, penguin.penguin_timer.t_min, penguin.penguin_timer.t_max);

	const float penguin_time_scale_min = 0.1f;
	const float penguin_time_scale_max = 3.0f;
	ImGui::SliderFloat("Penguin Time scale", &penguin.penguin_timer.scale, penguin_time_scale_min, penguin_time_scale_max);

	ImGui::Checkbox("Trajectory", &gui_scene.trajectory);
}


#endif
