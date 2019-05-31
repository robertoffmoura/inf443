#include "trajectory.hpp"

using namespace vcl;

static size_t index_at_value(float t, const std::vector<float>& vt);

static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);
static vec3 cardinal_spline_interpolation_derivative(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);

static vec3 cardinal_spline_interpolation(float t, std::vector<vcl::vec3> keyframe_position, std::vector<float> keyframe_time);
static vec3 cardinal_spline_interpolation_derivative(float t, std::vector<vcl::vec3> keyframe_position, std::vector<float> keyframe_time);


void Trajectory::setup() {
	// Initial Keyframe data
	//keyframe_position = {{0,0,0}, {3,0,0}, {3,3,0}, {6,3,0}, {9,3,0}, {9,0,0}, {12,0,0}, {12,-3,0}, {0,-3,0}, {0,0,0}, {3,0,0}, {3,3,0}};
	const size_t N = 9;
	for (size_t i=0; i<N+3; i++) {
		const float u = 14*sin(i /(1.0*N) * 2*M_PI );
		const float v = 14*cos(i /(1.0*N) * 2*M_PI );
		keyframe_position.push_back({u, v, 2});
	}
	keyframe_time = {0,1,2,3,4,5,6,7,8,9,10,11};
	// Set timer bounds
	//  To ease spline interpolation of a closed curve time \in [t_1,t_{N-2}]
	timer.t_min = keyframe_time[1];
	timer.t_max = keyframe_time[keyframe_time.size()-2];
	timer.t = timer.t_min;
	timer.scale = 0.160f;

	// Prepare the visual elements
	//surface = mesh_primitive_sphere();
	//surface.uniform_parameter.color   = {0,0,1};
	//surface.uniform_parameter.scaling = 0.08f;

	sphere = mesh_primitive_sphere();
	sphere.uniform_parameter.color = {1,1,1};
	sphere.uniform_parameter.scaling = 0.05f;

	segment_drawer.init();

	trajectory = curve_dynamic_drawable(100); // number of steps stroed in the trajectory
	trajectory.uniform_parameter.color = {0,0,1};

	picked_object=-1;
}

void Trajectory::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool show) {
	timer.update();
	if (!show) return;
	const float t = timer.t;
	// ********************************************* //
	// Compute interpolated position at time t
	// ********************************************* //

	// Assume a closed curve trajectory
	const size_t N = keyframe_time.size();


	const vec3 p = cardinal_spline_interpolation(t, keyframe_position, keyframe_time);
	trajectory.add_point(p);

	// Draw current position
	//surface.uniform_parameter.translation = p;
	//surface.draw(shaders["mesh"],scene.camera);


	// Draw sphere at each keyframe position
	for(size_t k=0; k<N; ++k)
	{
		const vec3& p_keyframe = keyframe_position[k];
		sphere.uniform_parameter.translation = p_keyframe;
		sphere.draw(shaders["mesh"],scene.camera);
	}


	// Draw segments between each keyframe
	for(size_t k=0; k<keyframe_position.size()-1; ++k)
	{
		const vec3& pa = keyframe_position[k];
		const vec3& pb = keyframe_position[k+1];

		segment_drawer.uniform_parameter.p1 = pa;
		segment_drawer.uniform_parameter.p2 = pb;
		segment_drawer.draw(shaders["segment_im"], scene.camera);
	}

	// Draw moving point trajectory
	trajectory.draw(shaders["curve"], scene.camera);

	// Draw selected sphere in red
	if( picked_object!=-1 )
	{
		const vec3& p_keyframe = keyframe_position[picked_object];
		sphere.uniform_parameter.color = vec3(1,0,0);
		sphere.uniform_parameter.scaling = 0.06f;
		sphere.uniform_parameter.translation = p_keyframe;
		sphere.draw(shaders["mesh"],scene.camera);
		sphere.uniform_parameter.color = vec3(1,1,1);
		sphere.uniform_parameter.scaling = 0.05f;
	}
}

void Trajectory::mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods) {
	
	// Mouse click is used to select a position of the control polygon
	// ******************************************************************** //

	// Window size
	int w=0;
	int h=0;
	glfwGetWindowSize(window, &w, &h);

	// Current cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Convert pixel coordinates to relative screen coordinates between [-1,1]
	const float x = 2*float(xpos)/float(w)-1;
	const float y = 1-2*float(ypos)/float(h);

	// Check if shift key is pressed
	const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

	if(action==GLFW_PRESS && key_shift)
	{
		// Create the 3D ray passing by the selected point on the screen
		const ray r = picking_ray(scene.camera, x,y);

		// Check if this ray intersects a position (represented by a sphere)
		//  Loop over all positions and get the intersected position (the closest one in case of multiple intersection)
		const size_t N = keyframe_position.size();
		picked_object = -1;
		float distance_min = 0.0f;
		for(size_t k=0; k<N; ++k)
		{
			const vec3 c = keyframe_position[k];
			const picking_info info = ray_intersect_sphere(r, c, 0.1f);

			if( info.picking_valid ) // the ray intersects a sphere
			{
				const float distance = norm(info.intersection-r.p); // get the closest intersection
				if( picked_object==-1 || distance<distance_min ){
					picked_object = k;
				}
			}
		}
	}
	
}

void Trajectory::mouse_move(scene_structure& scene, GLFWwindow* window) {
	
	// Mouse move is used to translate a position once selected
	// ******************************************************************** //

	// Window size
	int w=0;
	int h=0;
	glfwGetWindowSize(window, &w, &h);

	// Current cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Convert pixel coordinates to relative screen coordinates between [-1,1]
	const float x = 2*float(xpos)/float(w)-1;
	const float y = 1-2*float(ypos)/float(h);

	// Check that the mouse is clicked (drag and drop)
	const bool mouse_click_left  = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT )==GLFW_PRESS);
	const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

	const size_t N = keyframe_position.size();
	if(mouse_click_left && key_shift && picked_object!=-1)
	{
		// Translate the selected object to the new pointed mouse position within the camera plane
		// ************************************************************************************** //

		// Get vector orthogonal to camera orientation
		const mat4 M = scene.camera.camera_matrix();
		const vec3 n = {M(0,2),M(1,2),M(2,2)};

		// Compute intersection between current ray and the plane orthogonal to the view direction and passing by the selected object
		const ray r = picking_ray(scene.camera, x,y);
		vec3& p0 = keyframe_position[picked_object];
		const picking_info info = ray_intersect_plane(r,n,p0);

		// translate the position
		p0 = info.intersection;

		// Make sure that duplicated positions are moved together
		int Ns = N;
		if(picked_object==0 || picked_object==Ns-3){
			keyframe_position[0] = info.intersection;
			keyframe_position[N-3] = info.intersection;
		}
		if(picked_object==1 || picked_object==Ns-2){
			keyframe_position[1] = info.intersection;
			keyframe_position[N-2] = info.intersection;
		}
		if(picked_object==2 || picked_object==Ns-1){
			keyframe_position[2] = info.intersection;
			keyframe_position[N-1] = info.intersection;
		}

	}
	
}

vec3 Trajectory::position() {
	const float t = this->timer.t;
	return cardinal_spline_interpolation(t, keyframe_position, keyframe_time);
}

vec3 Trajectory::position_derivative() {
	const float t = this->timer.t;
	return cardinal_spline_interpolation_derivative(t, keyframe_position, keyframe_time);
}

static size_t index_at_value(float t, const std::vector<float>& vt) {
	const size_t N = vt.size();
	assert(vt.size()>=2);
	assert(t>=vt[0]);
	assert(t<vt[N-1]);

	size_t k=0;
	while( vt[k+1]<t )
		++k;
	return k;
}

static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3) {
	const float s = (t-t1)/(t2-t1);

	const float mu = 0.5;

	const vec3 d1 = mu * ((t3-t1)/(t2-t0)) * (p2-p0);
	const vec3 d2 = mu * ((t2-t0)/(t3-t1)) * (p3-p1);

	const vec3 p = (2*s*s*s - 3*s*s + 1)*p1 + (s*s*s - 2*s*s + s)*d1 + (-2*s*s*s + 3*s*s)*p2 + (s*s*s - s*s)*d2;

	return p;
}


static vec3 cardinal_spline_interpolation_derivative(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3) {
	const float s = (t-t1)/(t2-t1);
	const float dsdt = 1.0f/(t2-t1);

	const float mu = 0.5;

	const vec3 d1 = mu * ((t3-t1)/(t2-t0)) * (p2-p0);
	const vec3 d2 = mu * ((t2-t0)/(t3-t1)) * (p3-p1);

	const vec3 d = (6*s*s*dsdt - 6*s*dsdt)*p1 + (3*s*s*dsdt - 4*s*dsdt + dsdt)*d1 + (-6*s*s*dsdt + 6*s*dsdt)*p2 + (3*s*s*dsdt - 2*s*dsdt)*d2;

	return d;
}

static vec3 cardinal_spline_interpolation(float t, std::vector<vcl::vec3> keyframe_position, std::vector<float> keyframe_time) {
	const size_t idx = index_at_value(t, keyframe_time);
	// Linear interpolation
	/*
	const float t1 = keyframe_time[idx];
	const float t2 = keyframe_time[idx+1];

	const vec3& p1 = keyframe_position[idx];
	const vec3& p2 = keyframe_position[idx+1];


	const vec3 p = linear_interpolation(t,t1,t2,p1,p2);
	trajectory.add_point(p);
	*/

	// Cardianl Spline interpolation
	const float t0 = keyframe_time[idx-1];
	const float t1 = keyframe_time[idx];
	const float t2 = keyframe_time[idx+1];
	const float t3 = keyframe_time[idx+2];

	const vec3& p0 = keyframe_position[idx-1];
	const vec3& p1 = keyframe_position[idx];
	const vec3& p2 = keyframe_position[idx+1];
	const vec3& p3 = keyframe_position[idx+2];
	const vec3 p = cardinal_spline_interpolation(t,t0,t1,t2,t3,p0,p1,p2,p3);

	return p;
}

static vec3 cardinal_spline_interpolation_derivative(float t, std::vector<vcl::vec3> keyframe_position, std::vector<float> keyframe_time) {
	const size_t idx = index_at_value(t, keyframe_time);

	const float t0 = keyframe_time[idx-1];
	const float t1 = keyframe_time[idx];
	const float t2 = keyframe_time[idx+1];
	const float t3 = keyframe_time[idx+2];

	const vec3& p0 = keyframe_position[idx-1];
	const vec3& p1 = keyframe_position[idx];
	const vec3& p2 = keyframe_position[idx+1];
	const vec3& p3 = keyframe_position[idx+2];
	const vec3 p = cardinal_spline_interpolation_derivative(t,t0,t1,t2,t3,p0,p1,p2,p3);

	return p;
}
