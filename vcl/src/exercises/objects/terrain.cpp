#include "terrain.hpp"

using namespace vcl;

void Terrain::setup(gui_scene_structure& gui_scene) {
	// Load a texture image on GPU and stores its ID
	terrain_texture_id = texture_gpu( image_load_png("data/grass.png") );
	sand_terrain_texture_id = texture_gpu( image_load_png("data/sand.png") );

	// Clear memory in case of pre-existing terrain
	terrain.data_gpu.clear();
	sand_terrain.data_gpu.clear();

	// Create visual terrain surface
	terrain = create_terrain(gui_scene);
	terrain.uniform_parameter.color = {1.0f, 1.0f, 1.0f};
	terrain.uniform_parameter.shading.specular = 0.0f;

	sand_terrain = create_sand_terrain(gui_scene);
	sand_terrain.uniform_parameter.color = {1.7f, 1.7f, 1.7f};
	sand_terrain.uniform_parameter.shading.specular = 0.0f;
}

void Terrain::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
	// Before displaying a textured surface: bind the associated texture id
	glBindTexture(GL_TEXTURE_2D, terrain_texture_id);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

	// Display terrain
	//terrain.draw(shaders["mesh"], scene.camera);
	
	glBindTexture(GL_TEXTURE_2D, sand_terrain_texture_id);
	sand_terrain.draw(shaders["mesh"], scene.camera);

	// After the surface is displayed it is safe to set the texture id to a white image
	//  Avoids to use the previous texture for another object
	glBindTexture(GL_TEXTURE_2D, scene.texture_white);
	if (wireframe) { // wireframe if asked from the GUI
		glPolygonOffset( 1.0, 1.0 );
		terrain.draw(shaders["wireframe"], scene.camera);
		//sand_terrain.draw(shaders["wireframe"], scene.camera);
	}
}

// Generate terrain mesh
mesh Terrain::create_terrain(const gui_scene_structure& gui_scene) {
	// Number of samples of the terrain is N x N
	const size_t N = 200;

	mesh terrain; // temporary terrain storage (CPU only)
	terrain.position.resize(N*N);
	terrain.color.resize(N*N);

	// Fill terrain geometry
	for (size_t ku=0; ku<N; ++ku) {
		for (size_t kv=0; kv<N; ++kv) {
			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku/(N-1.0f);
			const float v = kv/(N-1.0f);

			// get gui parameters
			const float scaling = gui_scene.scaling;
			const int octave = gui_scene.octave;
			const float persistency = gui_scene.persistency;

			// Evaluate Perlin noise
			const float noise = perlin(scaling*u, scaling*v, octave, persistency);

			const float c = 0.3f+0.7f*noise;

			terrain.color[kv+N*ku]  = {c,c,c,1.0f};

			// Compute coordinates
			terrain.position[kv+N*ku] = evaluate_terrain(u,v, gui_scene);
			terrain.texture_uv.push_back(vec2(15*u, 15*v));
		}
	}


	// Generate triangle organization
	//  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
	const unsigned int Ns = N;
	for (unsigned int ku=0; ku<Ns-1; ++ku) {
		for (unsigned int kv=0; kv<Ns-1; ++kv) {
			const unsigned int idx = kv + N*ku; // current vertex offset

			const index3 triangle_1 = {idx, idx+1+Ns, idx+1};
			const index3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

			terrain.connectivity.push_back(triangle_1);
			terrain.connectivity.push_back(triangle_2);
		}
	}

	return terrain;
}

mesh Terrain::create_sand_terrain(const gui_scene_structure& gui_scene) {
	const size_t N = 200;

	mesh sand_terrain; // temporary terrain storage (CPU only)
	sand_terrain.position.resize(N*N);
	sand_terrain.color.resize(N*N);

		// Fill sand terrain geometry
	for (size_t ku=0; ku<N; ++ku) {
		for (size_t kv=0; kv<N; ++kv) {
			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku/(N-1.0f);
			const float v = kv/(N-1.0f);

			// get gui parameters
			const float scaling = gui_scene.scaling;
			const int octave = gui_scene.octave;
			const float persistency = gui_scene.persistency;

			// Evaluate Perlin noise
			const float noise = perlin(scaling*u, scaling*v, octave, persistency);

			const float c = 0.3f+0.7f*noise;

			sand_terrain.color[kv+N*ku]  = {c,c,c,1.0f};

			// Compute coordinates
			sand_terrain.position[kv+N*ku] = evaluate_terrain(u,v, gui_scene);
			sand_terrain.texture_uv.push_back(vec2(15*u, 15*v));
		}
	}

	// Generate triangle organization
	//  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
	const unsigned int Ns = N;
	for (unsigned int ku=0; ku<Ns-1; ++ku) {
		for (unsigned int kv=0; kv<Ns-1; ++kv) {
			const unsigned int idx = kv + N*ku; // current vertex offset

			const index3 triangle_1 = {idx, idx+1+Ns, idx+1};
			const index3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

			sand_terrain.connectivity.push_back(triangle_1);
			sand_terrain.connectivity.push_back(triangle_2);
		}
	}

	return sand_terrain;
}

float Terrain::evaluate_terrain_z_h_p_sigma(float u, float v, float h, float sigma, vec2 p) {
	const float d = norm(vec2(u,v)-p)/sigma;
	const float z = h*std::exp(-d*d);
	return z;
}


// Evaluate height of the terrain for any (u,v) \in [0,1]
float Terrain::evaluate_terrain_z(float u, float v) {
	const vec2 u0 = {0.35f, 0.45f};
	const float h0 = 1.7f/2.0;
	const float sigma0 = 0.15/2.0f;

	const vec2 u1 = {0.6f, 0.6f};
	const float h1 = -1.3f/2.0;
	const float sigma1 = 0.25/2.0f;

	const vec2 u2 = {0.6f, 0.35f};
	const float h2 = 1.5f/2.0f;
	const float sigma2 = 0.20f/2.0;

	const vec2 u3 = {0.5f, 0.5f};
	const float h3 = 4.0f;
	const float sigma3 = 0.25f;

	float z = 0;
	z += evaluate_terrain_z_h_p_sigma(u, v, h0, sigma0, u0);
	z += evaluate_terrain_z_h_p_sigma(u, v, h1, sigma1, u1);
	z += evaluate_terrain_z_h_p_sigma(u, v, h2, sigma2, u2);
	z += evaluate_terrain_z_h_p_sigma(u, v, h3, sigma3, u3);

	return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 Terrain::evaluate_terrain(float u, float v, const gui_scene_structure& gui_scene) {
	// get gui parameters
	const float scaling = gui_scene.scaling;
	const int octave = gui_scene.octave;
	const float persistency = gui_scene.persistency;
	const float height = gui_scene.height;

	// Evaluate Perlin noise
	const float noise = perlin(scaling*u, scaling*v, octave, persistency);

	// 3D vertex coordinates
	const float x = 40*(u-0.5f);
	const float y = 40*(v-0.5f);
	const float z = evaluate_terrain_z(u,v) + height*noise;
	//const float z = evaluate_terrain_z(u,v);
	
	return {x,y,z};
}
