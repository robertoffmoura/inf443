#include "mesh.hpp"

namespace vcl
{

mesh create_cylinder(float radius, float height) {
	const size_t N = 20;

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
	for(unsigned int ku=0; ku<N-1; ++ku)
	{
		const index3 triangle = {2*ku, 2*ku + 1, 2*ku + 2};
		const index3 triangle2 = {2*ku + 3, 2*ku + 2, 2*ku + 1};
		cylinder.connectivity.push_back(triangle);
		cylinder.connectivity.push_back(triangle2);
	}
	const index3 triangle = {2*N-2, 2*N-1, 0};
	const index3 triangle2 = {1,     0, 2*N-1};
	cylinder.connectivity.push_back(triangle);
	cylinder.connectivity.push_back(triangle2);

	return cylinder;
}

// Evaluate 3D position of the cylinder for any (u,v) \in [0,1]
vec3 evaluate_cylinder(float u, float v, int ku, float radius, float height) {
	const float x = radius*u;
	const float y = radius*v;
	const float z = ku%2 * height;

	return {x,y,z};
}

mesh create_cone(float radius, float height, float z_offset) {
	const size_t N = 20;

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
	for (unsigned int ku=0; ku<N-1; ++ku) {
		const index3 triangle = {(ku+1), ku, N};
		const index3 triangle2 = {(ku+1), ku, N+1};
		cone.connectivity.push_back(triangle);
		cone.connectivity.push_back(triangle2);
	}
	const index3 triangle = {0, N-1, N};
	const index3 triangle2 = {0, N-1, N+1};
	cone.connectivity.push_back(triangle);
	cone.connectivity.push_back(triangle2);

	return cone;
}


}