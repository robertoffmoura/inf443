#pragma once

#include "../../vcl/core/shape/mesh/mesh_structure/mesh.hpp"

namespace vcl {

mesh create_cylinder(float radius, float height);

// Evaluate 3D position of the cylinder for any (u,v) \in [0,1]
vec3 evaluate_cylinder(float u, float v, int ku, float radius, float height);

mesh create_cone(float radius, float height, float z_offset);


}