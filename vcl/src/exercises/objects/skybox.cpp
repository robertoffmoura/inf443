#include "skybox.hpp"

using namespace vcl;

mesh create_skybox();

void Skybox::setup() {
	//sky_texture_id = texture_gpu( image_load_png("data/skybox3.png") );
	skybox_texture_id = texture_gpu( image_load_png("data/skybox2.png") );

	skybox = create_skybox();
	skybox.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
	skybox.uniform_parameter.translation = {1, 1, 1};
	skybox.uniform_parameter.scaling = 150;

	/*
	mesh sky_surface_cpu;
	sky_surface_cpu.position     = {{-0.2f,-0.2f,0}, { 0.2f,-0.2f,0}, { 0.2f, 0.2f, 0}, {-0.2f, 0.2f, 0}};
	sky_surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
	sky_surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

	sky_surface = sky_surface_cpu;
	sky_surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
	*/
}


void Skybox::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
	// Display a billboard always facing the camera direction
	// ********************************************************** //
	glBindTexture(GL_TEXTURE_2D, skybox_texture_id);

	skybox.uniform_parameter.translation = scene.camera.camera_position() + vec3(0,0,-25.0f);

	skybox.draw(shaders["mesh"], scene.camera);
	if (wireframe) {
		skybox.draw(shaders["wireframe"], scene.camera);
	}
	/*
	glBindTexture(GL_TEXTURE_2D, sky_texture_id);

	sky_surface.uniform_parameter.translation = {0,0,0};
	sky_surface.uniform_parameter.translation = -30*scene.camera.orientation.col(2);
	sky_surface.uniform_parameter.scaling = 100 + scene.camera.scale*3.5f;
	sky_surface.uniform_parameter.rotation = scene.camera.orientation;
	//std::cout << scene.camera.orientation.col(2) << std::endl;
	sky_surface.draw(shaders["mesh"], scene.camera);
	if(gui_scene.wireframe)
		sky_surface.draw(shaders["wireframe"], scene.camera);
	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
	*/
}

mesh create_skybox() {
    const vec3 p000 = {-1,-1, 1};
    const vec3 p001 = {-1, 1, 1};
    const vec3 p010 = {-1,-1,-1};
    const vec3 p011 = {-1, 1,-1};
    const vec3 p100 = { 1,-1, 1};
    const vec3 p101 = { 1, 1, 1};
    const vec3 p110 = { 1,-1,-1};
    const vec3 p111 = { 1, 1,-1};

    mesh skybox;

    skybox.position = {
        p000, p100, p110, p010,
        p010, p110, p111, p011,
        p100, p110, p111, p101,
        p000, p001, p010, p011,
        p001, p101, p111, p011,
        p000, p100, p101, p001
    };


    skybox.connectivity = {
        {0,1,2}, {0,2,3}, {4,5,6}, {4,6,7},
        {8,11,10}, {8,10,9}, {17,16,19}, {17,19,18},
        {23,22,21}, {23,21,20}, {13,12,14}, {13,14,15}
    };

    const float e = 1e-3f;
    const float u0 = 0.0f;
    const float u1 = 0.25f+e;
    const float u2 = 0.5f-e;
    const float u3 = 0.75f-e;
    const float u4 = 1.0f;
    const float v0 = 0.0f;
    const float v1 = 1.0f/3.0f+e;
    const float v2 = 2.0f/3.0f-e;
    const float v3 = 1.0f;
    skybox.texture_uv = {
        {u1,v1}, {u2,v1}, {u2,v2}, {u1,v2},
        {u1,v2}, {u2,v2}, {u2,v3}, {u1,v3},
        {u2,v1}, {u2,v2}, {u3,v2}, {u3,v1},
        {u1,v1}, {u0,v1}, {u1,v2}, {u0,v2},
        {u4,v1}, {u3,v1}, {u3,v2}, {u4,v2},
        {u1,v1}, {u2,v1}, {u2,v0}, {u1,v0}
    };
    return skybox;
}