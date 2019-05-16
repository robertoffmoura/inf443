
#include "animated_interpolation.hpp"


#ifdef INF443_ANIMATED_INTERPOLATION


using namespace vcl;

mesh mesh_foot(float scale);

mesh mesh_primitive_ellipsoid(float radius0, float radius1, float radius2, const vec3& p0, size_t Nu, size_t Nv);

static size_t index_at_value(float t, const std::vector<float>& vt);

static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);
static vec3 cardinal_spline_interpolation_derivative(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);


void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    // Initial Keyframe data
    keyframe_position = {{0,0,0}, {3,0,0}, {3,3,0}, {6,3,0}, {9,3,0}, {9,0,0}, {12,0,0}, {12,-3,0}, {0,-3,0}, {0,0,0}, {3,0,0}, {3,3,0}};
    keyframe_time = {0,1,2,3,4,5,6,7,8,9,10,11};
    // Set timer bounds
    //  To ease spline interpolation of a closed curve time \in [t_1,t_{N-2}]
    timer.t_min = keyframe_time[1];
    timer.t_max = keyframe_time[keyframe_time.size()-2];
    timer.t = timer.t_min;

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



    const float r_head = 0.25f;
    const float r_body0 = 0.32f;
    const float r_body1 = 0.22f;
    const float r_body2 = 0.5f;
    const float foot_scale = 0.6f;

    mesh_drawable neck = mesh_primitive_sphere(0.01f,{0,0,0},20,20);

    mesh_drawable body = mesh_primitive_ellipsoid(r_body0, r_body1, r_body2, {0,0,0},40,40);
    body.uniform_parameter.color = {0,0,0};

    mesh_drawable belly = mesh_primitive_ellipsoid(0.8f*r_body0, 0.8f*r_body1, 0.8f*r_body2, {0,-0.08f,0},40,40);
    belly.uniform_parameter.color = {1.0f,1.0f,1.0f};

    mesh_drawable head = mesh_primitive_sphere(r_head,{0,0,0},40,40);
    head.uniform_parameter.color = {0,0,0};

    mesh_drawable eye = mesh_primitive_sphere(0.05f,{0,0,0},20,20);
    eye.uniform_parameter.color = {1.0f,1.0f,1.0f};
    mesh_drawable pupil = mesh_primitive_sphere(0.01f,{0,0,0},20,20);
    pupil.uniform_parameter.color = {0,0,0};

    mesh_drawable beak = mesh_primitive_cone(0.13f,  {0,0,0}, {0,0,0.3f}, 20, 20);
    beak.uniform_parameter.color = {1.0f, 0.7f, 0.4f};

    mesh_drawable arm_left = mesh_primitive_quad({0.2f,0,0.3f}, {0.4f,0,0.2f}, {0.4f,0,0}, {0.2f,0,-0.1f});
    mesh_drawable arm_lower_left = mesh_primitive_quad({0,0,0.2f}, {0.2f,0,0.1f}, {0.2f,0,0}, {0,0,0});
    mesh_drawable hand_left = mesh_primitive_quad({0,0,0.1f}, {0.03f,0,0.07f}, {0.03f,0,0.03f}, {0,0,0});

    mesh_drawable arm_right = mesh_primitive_quad({-0.2f,0,0.3f}, {-0.4f,0,0.2f}, {-0.4f,0,0}, {-0.2f,0,-0.1f});
    mesh_drawable arm_lower_right = mesh_primitive_quad({0,0,0.2f}, {-0.2f,0,0.1f}, {-0.2f,0,0}, {0,0,0});
    mesh_drawable hand_right = mesh_primitive_quad({0,0,0.1f}, {-0.03f,0,0.07f}, {-0.03f,0,0.03f}, {0,0,0});

    arm_left.uniform_parameter.color = {0,0,0};
    arm_lower_left.uniform_parameter.color = {0,0,0};
    hand_left.uniform_parameter.color = {0,0,0};
    arm_right.uniform_parameter.color = {0,0,0};
    arm_lower_right.uniform_parameter.color = {0,0,0};
    hand_right.uniform_parameter.color = {0,0,0};

    mesh_drawable foot = mesh_foot(foot_scale);
    foot.uniform_parameter.color = {1.0f, 0.7f, 0.4f};

    //mesh_drawable shoulder = mesh_primitive_sphere(0.055f);
    //shoulder.uniform_parameter.color = {0,0,0};

    hierarchy.add_element(body, "body", "root", {0,0,0});
    hierarchy.add_element(neck, "neck", "body", {0,0,0.4f});
    hierarchy.add_element(head, "head", "neck", {0,0.3f,0});
    hierarchy.add_element(belly, "belly", "body", {0,0,0});

    hierarchy.add_element(foot, "foot_left", "body", {0.2f,0,-0.4f}, rotation_from_axis_angle_mat3({0,0,1}, 0.5f));
    hierarchy.add_element(foot, "foot_right", "body", {-0.2f,0,-0.4f}, rotation_from_axis_angle_mat3({0,0,1}, -0.5f));

    hierarchy.add_element(eye, "eye_left", "head",{r_head/3,r_head/2,r_head/1.5f});
    hierarchy.add_element(eye, "eye_right", "head",{-r_head/3,r_head/2,r_head/1.5f});
    hierarchy.add_element(pupil, "pupil_left", "head",{1.2f*r_head/3, 1.2f*r_head/2, 1.2f*r_head/1.5f});
    hierarchy.add_element(pupil, "pupil_right", "head",{-1.2f*r_head/3, 1.2f*r_head/2, 1.2f*r_head/1.5f});
    hierarchy.add_element(beak, "beak", "head", {0,0,r_head/1.5f});

    hierarchy.add_element(arm_left, "arm_left", "body", {0, 0, -0.1f});
    hierarchy.add_element(arm_lower_left, "arm_lower_left", "arm_left", {0.4f,0,0});
    hierarchy.add_element(hand_left, "hand_left", "arm_lower_left", {0.2f,0,0});

    hierarchy.add_element(arm_right, "arm_right", "body", {0, 0, -0.1f});
    hierarchy.add_element(arm_lower_right, "arm_lower_right", "arm_right", {-0.4f,0,0});
    hierarchy.add_element(hand_right, "hand_right", "arm_lower_right", {-0.2f,0,0});

    //hierarchy.add_element(arm_top_left, "arm_top_left", "body",{-r_head+0.05f,0,0});
    //hierarchy.add_element(arm_bottom_left, "arm_bottom_left", "arm_top_left",{-l_arm,0,0});

    //hierarchy.add_element(arm_top_right, "arm_top_right", "body",{r_head-0.05f,0,0});
    //hierarchy.add_element(arm_bottom_right, "arm_bottom_right", "arm_top_right",{l_arm,0,0});

    //hierarchy.add_element(shoulder, "shoulder_left", "arm_bottom_left");
    //hierarchy.add_element(shoulder, "shoulder_right", "arm_bottom_right");

    penguin_timer.scale = 0.8f;
}




void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    const float t = timer.t;
    set_gui();



    // ********************************************* //
    // Compute interpolated position at time t
    // ********************************************* //
    const size_t idx = index_at_value(t, keyframe_time);

    // Assume a closed curve trajectory
    const size_t N = keyframe_time.size();



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







    penguin_timer.update();
    //set_gui();

    const float pt = penguin_timer.t;

    hierarchy.translation("body") =  {0,0.1f*(1+std::sin(2*3.14f*pt)),0};
    
    //hierarchy.translation("head") = {0,1,0};
    hierarchy.rotation("neck") = rotation_from_axis_angle_mat3({1,0,0}, 1 + 0.4f*std::sin(2*3.14f*(pt-0.4f)) );
    //hierarchy.translation("head") = {0,0,0};

    hierarchy.rotation("foot_left") = rotation_from_axis_angle_mat3({1,0,0}, -0.55f*std::sin(4*3.14f*(pt-0.4f)));
    hierarchy.rotation("foot_right") = rotation_from_axis_angle_mat3({1,0,0}, 0.55f*std::sin(4*3.14f*(pt-0.4f)));

    hierarchy.rotation("arm_left") = rotation_from_axis_angle_mat3({0,0,1}, 0.55f*std::sin(2*3.14f*(pt-0.4f)) );
    hierarchy.rotation("arm_lower_left") = rotation_from_axis_angle_mat3({0,0,1}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );
    hierarchy.rotation("hand_left") = rotation_from_axis_angle_mat3({0,0,1}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );

    hierarchy.rotation("arm_right") = rotation_from_axis_angle_mat3({0,0,-1}, 0.55f*std::sin(2*3.14f*(pt-0.4f)) );
    hierarchy.rotation("arm_lower_right") = rotation_from_axis_angle_mat3({0,0,-1}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );
    hierarchy.rotation("hand_right") = rotation_from_axis_angle_mat3({0,0,-1}, 0.3f*std::sin(2*3.14f*(pt-0.4f)) );

    hierarchy.translation("body") += p;
    const vec3 d = cardinal_spline_interpolation_derivative(t,t0,t1,t2,t3,p0,p1,p2,p3);
    hierarchy.rotation("body") = rotation_between_vector_mat3({0,0,1}, d);

    std::cout << d << std::endl;

    hierarchy.draw(shaders["mesh"], scene.camera);
    if(gui_scene.wireframe)
        hierarchy.draw(shaders["wireframe"], scene.camera);

}

void scene_exercise::mouse_click(scene_structure& scene, GLFWwindow* window, int , int action, int )
{
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

void scene_exercise::mouse_move(scene_structure& scene, GLFWwindow* window)
{
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

mesh mesh_foot(float scale) {
    mesh shape;

    shape.position.resize(11);

   	shape.position[0] = {-0.1f, 0, 0};
	shape.position[1] = {-0.4f, -0.4f, 0};
	shape.position[2] = {-0.15f, -0.3f, 0};
	shape.position[3] = {0, -0.5f, 0};
	shape.position[4] = {0.15f, -0.3f, 0};
	shape.position[5] = {0.4f, -0.4f, 0};
	shape.position[6] = {0.1f, 0, 0};
	shape.position[7] = {-0.1f, 0, 0.05f};
	shape.position[8] = {-0.15f, -0.3f, 0.05f};
	shape.position[9] = {0.15f, -0.3f, 0.05f};
	shape.position[10] = {0.1f, 0, 0.05f};

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

mesh mesh_primitive_ellipsoid(float radius0, float radius1, float radius2, const vec3& p0, size_t Nu, size_t Nv)
{
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


void scene_exercise::set_gui()
{
     ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);

    const float timek_scale_min = 0.1f;
    const float timek_scale_max = 3.0f;
    ImGui::SliderFloat("Time scale", &timer.scale, timek_scale_min, timek_scale_max);

    if( ImGui::Button("Print Keyframe") )
    {
        std::cout<<"keyframe_position={";
        for(size_t k=0; k<keyframe_position.size(); ++k)
        {
            const vec3& p = keyframe_position[k];
            std::cout<< "{"<<p.x<<"f,"<<p.y<<"f,"<<p.z<<"f}";
            if(k<keyframe_position.size()-1)
                std::cout<<", ";
        }
        std::cout<<"}"<<std::endl;
    }


    ImGui::SliderFloat("Penguin Time", &penguin_timer.t, penguin_timer.t_min, penguin_timer.t_max);

    const float penguin_time_scale_min = 0.1f;
    const float penguin_time_scale_max = 3.0f;
    ImGui::SliderFloat("Penguin Time scale", &penguin_timer.scale, penguin_time_scale_min, penguin_time_scale_max);
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);

}

static size_t index_at_value(float t, const std::vector<float>& vt)
{
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


#endif

