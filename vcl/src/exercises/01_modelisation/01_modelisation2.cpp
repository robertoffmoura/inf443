/*
#include "01_modelisation.hpp"

#ifdef INF443_01_MODELISATION

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
#include <random>
#include <iostream>




float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh create_terrain();

vec3 evaluate_cylinder(float u, float v, int ku, float radius, float height);
mesh create_cylinder(float radius, float height);

mesh create_cone(float radius, float height, float z_offset);
mesh create_tree_foliage(float radius, float height, float z_offset);

/*

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */

/*
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    update_tree_position();

    // Create visual terrain surface
    terrain = create_terrain();
    terrain.uniform_parameter.color = {0.6f,0.85f,0.5f};
    terrain.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

    // Create visual cylinder surface
    cylinder = create_cylinder(0.5f, 4.0f);
    cylinder.uniform_parameter.color = {0.396f,0.263f,0.129f};
    cylinder.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

    // Create visual cone surface
    //cone = create_cone(2.0f, 3.0f, 4.0f);
    //cone.uniform_parameter.color = {0.6f,0.85f,0.5f};
    //cone.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material

    // Create visual foliage surface
    foliage = create_tree_foliage(2.0f, 3.0f, 1.2f);
    foliage.uniform_parameter.color = {0.6f,0.85f,0.5f};
    foliage.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material


    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

}

*/

/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */

/*
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    terrain.draw(shaders["mesh"], scene.camera);


    for (size_t i=0; i< tree_position.size(); i++) {
        cylinder.uniform_parameter.translation = tree_position[i];
        cylinder.draw(shaders["mesh"], scene.camera);
        foliage.uniform_parameter.translation = tree_position[i];
        foliage.draw(shaders["mesh"], scene.camera);
    }

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        terrain.draw(shaders["wireframe"], scene.camera);
    }
}

float evaluate_terrain_z_h_p_sigma(float u, float v, float h, float sigma, vec2 p)
{
    const float d = norm(vec2(u,v)-p)/sigma;
    const float z = h*std::exp(-d*d);
    return z;
}


// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
    const vec2 u0 = {0.2f, 0.4f};
    const float h0 = 1.7f;
    const float sigma0 = 0.15f;

    const vec2 u1 = {0.7f, 0.7f};
    const float h1 = -1.3f;
    const float sigma1 = 0.25f;

    const vec2 u2 = {0.7f, 0.2f};
    const float h2 = 1.5f;
    const float sigma2 = 0.20f;

    float z = 0;
    z += evaluate_terrain_z_h_p_sigma(u, v, h0, sigma0, u0);
    z += evaluate_terrain_z_h_p_sigma(u, v, h1, sigma1, u1);
    z += evaluate_terrain_z_h_p_sigma(u, v, h2, sigma2, u2);

    return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z(u,v);

    return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute coordinates
            terrain.position[kv+N*ku] = evaluate_terrain(u,v);
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const index3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const index3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
}

// Evaluate 3D position of the cylinder for any (u,v) \in [0,1]
vec3 evaluate_cylinder(float u, float v, int ku, float radius, float height)
{
    const float x = radius*u;
    const float y = radius*v;
    const float z = ku%2 * height;

    return {x,y,z};
}

mesh create_cylinder(float radius, float height) {
    const size_t N = 10;
    
    mesh cylinder; // temporary terrain storage (CPU only)
    cylinder.position.resize(2*N);

    // Fill cylinder geometry
    for(size_t ku=0; ku<2*N; ++ku) {
        // Compute local parametric coordinates (u,v) \in [0,1]
        const float u = sin(ku/2*2 /(2.0*N) * 2*M_PI );
        const float v = cos(ku/2*2 /(2.0*N) * 2*M_PI );

        // Compute coordinates
        cylinder.position[ku] = evaluate_cylinder(u,v,ku, radius, height);
    }

    // Generate triangle organization
    for(unsigned int ku=0; ku<2*N-2; ++ku)
    {
        const index3 triangle = {ku, ku+1, ku+2};
        cylinder.connectivity.push_back(triangle);
    }
    const index3 triangle2 = {2*N-2, 2*N-1, 0};
    const index3 triangle3 = {2*N-1,     0, 1};
    cylinder.connectivity.push_back(triangle2);
    cylinder.connectivity.push_back(triangle3);

    return cylinder;
}



mesh create_cone(float radius, float height, float z_offset) {
    const size_t N = 10;
    
    mesh cone; // temporary terrain storage (CPU only)
    cone.position.resize(N+2);

    // Fill cylinder geometry
    for(size_t ku=0; ku<N; ++ku) {
        // Compute local parametric coordinates (u,v) \in [0,1]
        const float u = sin(ku /(N+0.0) * 2*M_PI );
        const float v = cos(ku /(N+0.0) * 2*M_PI );

        // Compute coordinates
        cone.position[ku] = {radius*u, radius*v, z_offset};
    }
    cone.position[N] = {0, 0, z_offset};
    cone.position[N+1] = {0, 0, z_offset + height};

    // Generate triangle organization
    for(unsigned int ku=0; ku<N-1; ++ku)
    {
        const index3 triangle = {ku, ku+1, N};
        const index3 triangle2 = {ku, ku+1, N+1};
        cone.connectivity.push_back(triangle);
        cone.connectivity.push_back(triangle2);
    }
    const index3 triangle =  {N-1, 0, N};
    const index3 triangle2 = {N-1, 0, N+1};
    cone.connectivity.push_back(triangle);
    cone.connectivity.push_back(triangle2);

    return cone;
}

mesh create_tree_foliage(float radius, float height, float z_offset)
{
    mesh m = create_cone(radius, height, 0);
    m.push_back( create_cone(radius, height, z_offset) );
    m.push_back( create_cone(radius, height, 2*z_offset) );

    return m;
}

void scene_exercise::update_tree_position() {
    std::uniform_real_distribution<float> distrib(0.0,1.0);
    std::default_random_engine generator;
    size_t number_of_trees = 10;

    for (size_t i=0; i<number_of_trees; i++) {
        float u = distrib(generator);
        float v = distrib(generator);
        tree_position.push_back(evaluate_terrain(u,v));
    }
}



void scene_exercise::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}



#endif

*/