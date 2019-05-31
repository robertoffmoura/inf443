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

/*
mesh create_cone_trunk(float r_top, float r_bottom, float height, vec3 translation) {
	const unsigned int N = 20;
	const unsigned int N2 = 20;

	mesh cylinder; // temporary terrain storage (CPU only)
	cylinder.position.resize(N2*N);

	// Fill cylinder geometry
	for (size_t i=0; i<N2; i++) {
		for (size_t ku=0; ku<N; ++ku) {
			//const float a = -angle + i/(1.0*(N2-1)) * 2*angle;
			vec3 center = {0, 0, i*height/(N2-1)};
			const float r = r_bottom + i*(r_top - r_bottom)/(N2-1);
			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = sin(ku /(1.0f*N) * 2*M_PI );
			const float v = cos(ku /(1.0f*N) * 2*M_PI );
			vec3 offset = {r * u, r * v, 0};
			// Compute coordinates
			cylinder.position[i*N + ku] = translation + center + offset;
			cylinder.texture_uv.push_back(vec2(ku/(1.0f*N), i/(N2-1.0f)));
		}
	}

	// Generate triangle organization
	for (unsigned int i=0; i<N2-1; i++) {
		for (unsigned int ku=0; ku<N-1; ++ku) {
			index3 triangle  = {i*N + ku,         (i+1)*N + ku,  i*N + ku + 1};
			index3 triangle2 = {(i+1)*N + ku + 1, i*N + ku + 1,  (i+1)*N + ku};
			cylinder.connectivity.push_back(triangle);
			cylinder.connectivity.push_back(triangle2);
		}
		index3 triangle  = {i*N + N-1, (i+1)*N + N-1,   i*N          };
		index3 triangle2 = {(i+1)*N  ,   i*N,           (i+1)*N + N-1};
		cylinder.connectivity.push_back(triangle);
		cylinder.connectivity.push_back(triangle2);
	}

	return cylinder;
}
*/

mesh create_cone_trunk(float r_top, float r_bottom, float height, vec3 translation) {
	const unsigned int N = 20 + 1;
	const unsigned int N2 = 20;

	mesh cylinder; // temporary terrain storage (CPU only)
	cylinder.position.resize(N2*N);

	// Fill cylinder geometry
	for (size_t i=0; i<N2; i++) {
		for (size_t ku=0; ku<N; ++ku) {
			//const float a = -angle + i/(1.0*(N2-1)) * 2*angle;
			vec3 center = {0, 0, i*height/(N2-1)};
			const float r = r_bottom + i*(r_top - r_bottom)/(N2-1);
			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = sin(ku /(N-1.0f) * 2*M_PI );
			const float v = cos(ku /(N-1.0f) * 2*M_PI );
			vec3 offset = {r * u, r * v, 0};
			// Compute coordinates
			cylinder.position[i*N + ku] = translation + center + offset;
			cylinder.texture_uv.push_back(vec2(ku/(N-1.0f), 1.0f - i/(N2-1.0f)));
		}
	}

	// Generate triangle organization
	for (unsigned int i=0; i<N2-1; i++) {
		for (unsigned int ku=0; ku<N-1; ++ku) {
			index3 triangle  = {i*N + ku,         (i+1)*N + ku,  i*N + ku + 1};
			index3 triangle2 = {(i+1)*N + ku + 1, i*N + ku + 1,  (i+1)*N + ku};
			cylinder.connectivity.push_back(triangle);
			cylinder.connectivity.push_back(triangle2);
		}
	}

	return cylinder;
}


}