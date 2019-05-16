#include "penguin.hpp"

using namespace vcl;

mesh mesh_foot(float scale);
mesh mesh_primitive_quad_scale(float scale, const vec3& p00, const vec3& p10, const vec3& p11, const vec3& p01);
mesh mesh_primitive_ellipsoid(float radius0, float radius1, float radius2, const vec3& p0, size_t Nu, size_t Nv);

void Penguin::setup(float scale) {
	const float r_head = 0.25f * scale;
	const float r_eye = 0.05f * scale;
	const float r_pupil = 0.01f * scale;
	const float r_body0 = 0.32f * scale;
	const float r_body1 = 0.5f * scale;
	const float r_body2 = 0.22f * scale;
	const float foot_scale = 0.6f * scale;
	const float arm = 1.0f * scale;

	mesh_drawable neck = mesh_primitive_sphere(0.01f,{0,0,0},20,20);

	mesh_drawable body = mesh_primitive_ellipsoid(r_body0, r_body1, r_body2, {0,0,0},40,40);
	body.uniform_parameter.color = {0,0,0};

	mesh_drawable belly = mesh_primitive_ellipsoid(0.8f*r_body0, 0.8f*r_body1, 0.8f*r_body2, {0,0,-0.3636f*r_body2},40,40);
	belly.uniform_parameter.color = {1.0f,1.0f,1.0f};

	mesh_drawable head = mesh_primitive_sphere(r_head,{0,0,0},40,40);
	head.uniform_parameter.color = {0,0,0};

	mesh_drawable eye = mesh_primitive_sphere(r_eye,{0,0,0},20,20);
	eye.uniform_parameter.color = {1.0f,1.0f,1.0f};
	mesh_drawable pupil = mesh_primitive_sphere(r_pupil,{0,0,0},20,20);
	pupil.uniform_parameter.color = {0,0,0};

	mesh_drawable beak = mesh_primitive_cone(0.52f*r_head,  {0,0,0}, {0,1.2f*r_head,0}, 20, 20);
	beak.uniform_parameter.color = {1.0f, 0.7f, 0.4f};

	mesh_drawable arm_left = mesh_primitive_quad_scale(arm, {0.2f,0.3f,0}, {0.4f,0.2f,0}, {0.4f,0,0}, {0.2f,-0.1f,0});
	mesh_drawable arm_lower_left = mesh_primitive_quad_scale(arm, {0,0.2f,0}, {0.2f,0.1f,0}, {0.2f,0,0}, {0,0,0});
	mesh_drawable hand_left = mesh_primitive_quad_scale(arm, {0,0.1f,0}, {0.03f,0.07f,0}, {0.03f,0.03f,0}, {0,0,0});

	mesh_drawable arm_right = mesh_primitive_quad_scale(arm, {-0.2f,0.3f,0}, {-0.4f,0.2f,0}, {-0.4f,0,0}, {-0.2f,-0.1f,0});
	mesh_drawable arm_lower_right = mesh_primitive_quad_scale(arm, {-0,0.2f,0}, {-0.2f,0.1f,0}, {-0.2f,0,0}, {-0,0,0});
	mesh_drawable hand_right = mesh_primitive_quad_scale(arm, {-0,0.1f,0}, {-0.03f,0.07f,0}, {-0.03f,0.03f,0}, {0,0,0});

	arm_left.uniform_parameter.color = {0,0,0};
	arm_lower_left.uniform_parameter.color = {0,0,0};
	hand_left.uniform_parameter.color = {0,0,0};
	arm_right.uniform_parameter.color = {0,0,0};
	arm_lower_right.uniform_parameter.color = {0,0,0};
	hand_right.uniform_parameter.color = {0,0,0};

	mesh_drawable foot = mesh_foot(foot_scale);
	foot.uniform_parameter.color = {1.0f, 0.7f, 0.4f};


	hierarchy.add_element(body, "body", "root", {0,0,0});
	hierarchy.add_element(neck, "neck", "body", {0,0.8f*r_body1,0});
	hierarchy.add_element(head, "head", "neck", {0,0,1.2f*r_head});
	hierarchy.add_element(belly, "belly", "body", {0,0,0});
	hierarchy.add_element(foot, "foot_left", "body", {0.625f*r_body0,-0.8f*r_body1,0}, rotation_from_axis_angle_mat3({0,1,0}, 0.5f));
	hierarchy.add_element(foot, "foot_right", "body", {-0.625f*r_body0,-0.8f*r_body1,0}, rotation_from_axis_angle_mat3({0,1,0}, -0.5f));

	hierarchy.add_element(eye, "eye_left", "head",{r_head/3,r_head/1.5f,r_head/2});
	hierarchy.add_element(eye, "eye_right", "head",{-r_head/3,r_head/1.5f,r_head/2});
	hierarchy.add_element(pupil, "pupil_left", "head",{1.2f*r_head/3, 1.2f*r_head/1.5f, 1.2f*r_head/2});
	hierarchy.add_element(pupil, "pupil_right", "head",{-1.2f*r_head/3, 1.2f*r_head/1.5f, 1.2f*r_head/2});
	hierarchy.add_element(beak, "beak", "head", {0,r_head/1.5f,0});

	hierarchy.add_element(arm_left, "arm_left", "body", {0, -0.2f*r_body1, 0});
	hierarchy.add_element(arm_lower_left, "arm_lower_left", "arm_left", {0.4f*arm,0,0});
	hierarchy.add_element(hand_left, "hand_left", "arm_lower_left", {0.2f*arm,0,0});

	hierarchy.add_element(arm_right, "arm_right", "body", {0, -0.1f*r_body1, 0});
	hierarchy.add_element(arm_lower_right, "arm_lower_right", "arm_right", {-0.4f*arm,0,0});
	hierarchy.add_element(hand_right, "hand_right", "arm_lower_right", {-0.2f*arm,0,0});

	penguin_timer.scale = 0.8f;
}

void Penguin::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe, vec3 position, vec3 derivative) {
	penguin_timer.update();
	//set_gui();
	const float pt = penguin_timer.t;

	hierarchy.translation("body") =  {0,0,0.06f*(1-std::sin(2*3.14f*pt))};

	//hierarchy.translation("head") = {0,1,0};
	hierarchy.rotation("neck") = rotation_from_axis_angle_mat3({1,0,0}, -1 + 0.4f*std::sin(2*3.14f*(pt-0.4f)) );
	//hierarchy.translation("head") = {0,0,0};

	hierarchy.rotation("foot_left") = rotation_from_axis_angle_mat3({1,0,0}, -0.55f*std::sin(4*3.14f*(pt-0.4f)));
	hierarchy.rotation("foot_right") = rotation_from_axis_angle_mat3({1,0,0}, 0.55f*std::sin(4*3.14f*(pt-0.4f)));

	hierarchy.rotation("arm_left") = rotation_from_axis_angle_mat3({0,1,0}, 0.55f*std::sin(2*3.14f*(pt-0.4f)) );
	hierarchy.rotation("arm_lower_left") = rotation_from_axis_angle_mat3({0,1,0}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );
	hierarchy.rotation("hand_left") = rotation_from_axis_angle_mat3({0,1,0}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );

	hierarchy.rotation("arm_right") = rotation_from_axis_angle_mat3({0,-1,0}, 0.55f*std::sin(2*3.14f*(pt-0.4f)) );
	hierarchy.rotation("arm_lower_right") = rotation_from_axis_angle_mat3({0,-1,0}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );
	hierarchy.rotation("hand_right") = rotation_from_axis_angle_mat3({0,-1,0}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );

	const vec3 p = position;
	hierarchy.translation("body") += p;
	const vec3 d = derivative;
	//hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2)*rotation_between_vector_mat3({0,0,1}, d);
	//hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2)*rotation_between_vector_mat3({0,1,0}, d);
	hierarchy.rotation("body") = rotation_between_vector_mat3({0,1,0}, d);

	//std::cout << d << std::endl;

	hierarchy.draw(shaders["mesh"], scene.camera);
	if (wireframe) {
		hierarchy.draw(shaders["wireframe"], scene.camera);
	}
}

mesh mesh_foot(float scale) {
	mesh shape;

	shape.position.resize(11);

   	shape.position[0] = {-0.1f, 0, 0};
	shape.position[1] = {-0.4f, 0, -0.4f};
	shape.position[2] = {-0.15f, 0, -0.3f};
	shape.position[3] = {0, 0, -0.5f};
	shape.position[4] = {0.15f, 0, -0.3f};
	shape.position[5] = {0.4f, 0, -0.4f};
	shape.position[6] = {0.1f, 0, 0};
	shape.position[7] = {-0.1f, 0.05f, 0};
	shape.position[8] = {-0.15f, 0.05f, -0.3f};
	shape.position[9] = {0.15f, 0.05f, -0.3f};
	shape.position[10] = {0.1f, 0.05f, 0};

	for (size_t i=0; i<shape.position.size(); i++) {
		shape.position[i] *= scale;
	}

	shape.connectivity.push_back({1, 2, 0});
	shape.connectivity.push_back({2, 3, 0});
	shape.connectivity.push_back({3, 6, 0});
	shape.connectivity.push_back({3, 4, 6});
	shape.connectivity.push_back({4, 5, 6});
	shape.connectivity.push_back({5, 9, 10});
	shape.connectivity.push_back({3, 8, 9});
	shape.connectivity.push_back({8, 1, 7});
	shape.connectivity.push_back({9, 8, 10});
	shape.connectivity.push_back({8, 7, 10});
	shape.connectivity.push_back({7, 0, 10});
	shape.connectivity.push_back({0, 6, 10});
	shape.connectivity.push_back({1, 0, 7});
	shape.connectivity.push_back({6, 5, 10});
	shape.connectivity.push_back({1, 8, 2});
	shape.connectivity.push_back({2, 8, 3});
	shape.connectivity.push_back({9, 4, 3});
	shape.connectivity.push_back({4, 9, 5});

	return shape;
}

mesh mesh_primitive_quad_scale(float scale, const vec3& p00, const vec3& p10, const vec3& p11, const vec3& p01) {
	return mesh_primitive_quad(scale*p00, scale*p10, scale*p11, scale*p01);
}


mesh mesh_primitive_ellipsoid(float radius0, float radius1, float radius2, const vec3& p0, size_t Nu, size_t Nv) {
	mesh shape;
	for( size_t ku=0; ku<Nu; ++ku ) {
		for( size_t kv=0; kv<Nv; ++kv ) {

			const float u = static_cast<float>(ku)/static_cast<float>(Nu-1);
			const float v = static_cast<float>(kv)/static_cast<float>(Nv);

			const float theta = static_cast<float>( 3.14159f*u );
			const float phi   = static_cast<float>( 2*3.14159f*v );

			const float x = radius0 * std::sin(theta) * std::cos(phi);
			const float y = radius1 * std::sin(theta) * std::sin(phi);
			const float z = radius2 * std::cos(theta);

			const vec3 p = {x,y,z};
			const vec3 n = normalize(p);

			shape.position.push_back( p+p0 );
			shape.normal.push_back( n );
		}
	}

	shape.connectivity = connectivity_grid(Nu, Nv, false, true);

	return shape;
}

