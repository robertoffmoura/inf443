#include "zebra.hpp"

using namespace vcl;

//functions for the moviment
float R(const float t);
float L(const float t);

//Pieces
mesh superior_back_joint(const float r0, const float halfl);
mesh superior_leg(const float r0, const float l);
mesh horse_body(const float r);
mesh create_cone_trunk(float r_top, float r_bottom, float height, vec3 translation);
mesh zebra_neck(const float r_neck, const float x_neck);
mesh zebra_ear(const float r_neck);

void Zebra::setup(){
    //Object Parametres
    const float r = 10.0f;
    const float l = 15.0f;
    const float l_head = 9.0f;
    const float x_neck = 5.0f;
    const float r_neck = 4.0f;

    //Math contants
    const float tg70 = 2.75f;
    //const float cos15 = 0.966f;
    //const float cos70 = 0.342f;
    //const float sen15 = 0.259f;
    const float sen10 = 0.174f;
    const float cos10 = 0.985f;
    const float cos30 = 0.866f;
    

    //test
    mesh zebra_body = horse_body(10.0f);

    hierarchy.scaling = 0.2f;
    hierarchy.add_element(zebra_body, "body","root", {0,0,0});

    //legs

    //Frontal_Right
    mesh Superior_joint_FR = mesh_primitive_sphere(2.0f);
    mesh SuperiorLeg_FR = mesh_primitive_cylinder(2.0f,{0,-l/2,0},{0,0,0});
    mesh Inferior_joint_FR = mesh_primitive_sphere(2.0f);
    mesh InferiorLeg_FR = mesh_primitive_cylinder(2.0f,{0,-l/2 - 1.73f,0},{0,0,0});
    mesh Paw_FR = mesh_primitive_cone(3.0f,{0,0,0}, {0,5.19f, 0});

    hierarchy.add_element(Superior_joint_FR, "Superior_joint_FR","body", {0.97f*r,-0.4f*r,0.5f*r});
    hierarchy.add_element(SuperiorLeg_FR, "SuperiorLeg_FR","Superior_joint_FR", {0,0,0});
    hierarchy.add_element(Inferior_joint_FR, "Inferior_joint_FR","SuperiorLeg_FR", {0,-l/2,0});
    hierarchy.add_element(InferiorLeg_FR, "InferiorLeg_FR","Inferior_joint_FR", {0,0,0});
    hierarchy.add_element(Paw_FR, "Paw_FR","InferiorLeg_FR", {0,-l/2 - 1.73f,0});

    //Frontal_Left
    mesh Superior_joint_FL = mesh_primitive_sphere(2.0f);
    mesh SuperiorLeg_FL = mesh_primitive_cylinder(2.0f,{0,-l/2,0},{0,0,0});
    mesh Inferior_joint_FL = mesh_primitive_sphere(2.0f);
    mesh InferiorLeg_FL = mesh_primitive_cylinder(2.0f,{0,-l/2 - 1.73f,0},{0,0,0});
    mesh Paw_FL = mesh_primitive_cone(3.0f,{0,0,0}, {0,5.19f, 0});

    hierarchy.add_element(Superior_joint_FL, "Superior_joint_FL","body", {0.97f*r,-0.4f*r,-0.5f*r});
    hierarchy.add_element(SuperiorLeg_FL, "SuperiorLeg_FL","Superior_joint_FL", {0,0,0});
    hierarchy.add_element(Inferior_joint_FL, "Inferior_joint_FL","SuperiorLeg_FL", {0,-l/2,0});
    hierarchy.add_element(InferiorLeg_FL, "InferiorLeg_FL","Inferior_joint_FL", {0,0,0});
    hierarchy.add_element(Paw_FL, "Paw_FL","InferiorLeg_FL", {0,-l/2 - 1.73f,0});

    //Back_Right
    mesh Superior_joint_BR =  superior_back_joint(2.0f, l/2);
    mesh SuperiorLeg_BR = superior_leg(2.0f, l);
    mesh Inferior_joint_BR = mesh_primitive_sphere(2.0f);
    mesh InferiorLeg_BR = mesh_primitive_cylinder(2.0f,{0,-l/2 - 1.73f,0},{0,0,0});
    mesh Paw_BR = mesh_primitive_cone(3.0f,{0,0,0}, {0,5.19f, 0});

    hierarchy.add_element(Superior_joint_BR, "Superior_joint_BR","body", {-0.85f*r,-0.2f*r,0.45f*r});
    hierarchy.add_element(SuperiorLeg_BR, "SuperiorLeg_BR","Superior_joint_BR", {0,0,0});
    hierarchy.add_element(Inferior_joint_BR, "Inferior_joint_BR","SuperiorLeg_BR", {0,-l/2,0});
    hierarchy.add_element(InferiorLeg_BR, "InferiorLeg_BR","Inferior_joint_BR", {0,0,0});
    hierarchy.add_element(Paw_BR, "Paw_BR","InferiorLeg_BR", {0,-l/2 - 1.73f,0});

    //Back_Left 
    mesh Superior_joint_BL = superior_back_joint(2.0f, l/2);;
    mesh SuperiorLeg_BL = superior_leg(2.0f, l);
    mesh Inferior_joint_BL = mesh_primitive_sphere(2.0f);
    mesh InferiorLeg_BL = mesh_primitive_cylinder(2.0f,{0,-l/2 - 1.73f,0},{0,0,0});
    mesh Paw_BL = mesh_primitive_cone(3.0f,{0,0,0}, {0,5.19f, 0});

    hierarchy.add_element(Superior_joint_BL, "Superior_joint_BL","body", {-0.85f*r,-0.2f*r,-0.45f*r});
    hierarchy.add_element(SuperiorLeg_BL, "SuperiorLeg_BL","Superior_joint_BL", {0,0,0});
    hierarchy.add_element(Inferior_joint_BL, "Inferior_joint_BL","SuperiorLeg_BL", {0,-l/2,0});
    hierarchy.add_element(InferiorLeg_BL, "InferiorLeg_BL","Inferior_joint_BL", {0,0,0});
    hierarchy.add_element(Paw_BL, "Paw_BL","InferiorLeg_BL", {0,-l/2 - 1.73f,0});

    //Neck
    mesh neck = zebra_neck(r_neck, x_neck);
    hierarchy.add_element(neck,"neck", "body", {0.97f*r,0.4f*r,0});

    //head
    mesh head_1 = mesh_primitive_sphere(r_neck - 0.05f);//0.05f it's a correction factor
    mesh head_2 = create_cone_trunk(0.8f*r_neck, r_neck, l_head, {0,0,0});
    mesh head_3 = mesh_primitive_sphere(0.8f*r_neck - 0.04f);

    hierarchy.add_element(head_1,"head_1","neck",{x_neck, tg70*x_neck,0});
    hierarchy.add_element(head_2,"head_2","head_1",{0,0,0});
    hierarchy.add_element(head_3,"head_3","head_2",{0,l_head,0});

 
    //Acessories
    
    //Ears
    mesh L_ear = zebra_ear(1.5f*r_neck);
    mesh R_ear = zebra_ear(1.5f*r_neck);

    hierarchy.add_element(L_ear, "L_ear", "head_1", { -r_neck*cos30*cos10 + 0.15f, -r_neck*cos30*sen10, r_neck*0.5f});
    hierarchy.add_element(R_ear, "R_ear", "head_1", {-r_neck*cos30*cos10 + 0.2f, -r_neck*cos30*sen10,-r_neck*0.5f});
    
    //Eyes
    mesh L_eye = mesh_primitive_sphere(0.3f);
    mesh R_eye = mesh_primitive_sphere(0.3f);
   
    hierarchy.add_element(L_eye, "L_eye", "head_2", {-cos30*19.0f*r_neck/20.0f,l_head/4.0f,-19.0f*r_neck/40.0f });
    hierarchy.add_element(R_eye, "R_eye", "head_2", {-cos30*19.0f*r_neck/20.0f,l_head/4.0f,19.0f*r_neck/40.0f });
    hierarchy.mesh_visual("L_eye").uniform_parameter.color = {0,0,0};
    hierarchy.mesh_visual("R_eye").uniform_parameter.color = {0,0,0};

	hierarchy.scaling = 0.01f;
    
    zebra_timer.scale = 0.5f;
}

void Zebra::draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool wireframe) {
    const float Pi = 3.14159;
    zebra_timer.update();

    const float t = zebra_timer.t;

    
    //correcting orientations
    hierarchy.rotation("head_1") = rotation_from_axis_angle_mat3({0,0,-1},7.0f* Pi/12.0f);
    hierarchy.rotation("R_ear") = rotation_from_axis_angle_mat3({0,-1,0}, Pi/6.0f);
    hierarchy.rotation("L_ear") = rotation_from_axis_angle_mat3({0,1,0}, Pi/6.0f);
    
    //Rotations
    hierarchy.rotation("SuperiorLeg_FR") = rotation_from_axis_angle_mat3({0,0,-1}, R(t) );
    hierarchy.rotation("InferiorLeg_FR") = rotation_from_axis_angle_mat3({0,0,-1}, std::max(std::sin(-2*3.14f*(t-0.4f)), 0.3f) );
    hierarchy.rotation("Superior_joint_BR") = rotation_from_axis_angle_mat3({0,0,-1}, R(t) );
    hierarchy.rotation("InferiorLeg_BR") = rotation_from_axis_angle_mat3({0,0,-1}, std::max(std::sin(-2*3.14f*(t-0.4f)), 0.3f) );
    hierarchy.rotation("SuperiorLeg_FL") = rotation_from_axis_angle_mat3({0,0,-1}, L(t) );
    hierarchy.rotation("InferiorLeg_FL") = rotation_from_axis_angle_mat3({0,0,-1}, std::max(std::sin(2*3.14f*(t-0.4f)), 0.3f) );
    hierarchy.rotation("Superior_joint_BL") = rotation_from_axis_angle_mat3({0,0,-1}, L(t) );
    hierarchy.rotation("InferiorLeg_BL") = rotation_from_axis_angle_mat3({0,0,-1}, std::max(std::sin(2*3.14f*(t-0.4f)), 0.3f) );
    
    //Translation
	const vec3 p = {0,0,5.0f};
    hierarchy.translation("body") = p + vec3(0,0,0.02f*(1+std::sin(2*3.14f*t)));
	hierarchy.rotation("body") = rotation_from_axis_angle_mat3({1,0,0}, M_PI/2.0f);

    hierarchy.draw(shaders["mesh"], scene.camera);
	if (wireframe) {
		hierarchy.draw(shaders["wireframe"], scene.camera);
	}
}

float R(const float t){
    if(std::sin(2*3.14f*(t-0.4f)) > 0){
        return 0.5f*std::sin(2*3.14f*(t-0.4f));
    }
    else{
        return std::sin(2*3.14f*(t-0.4f));
    }
}

float L(const float t){
    if(std::sin(2*3.14f*(t-0.4f)) > 0){
        return std::sin(-2*3.14f*(t-0.4f));
    }
    else{
        return 0.5f*std::sin(-2*3.14f*(t-0.4f));
    }
}

mesh superior_back_joint(const float r0, const float halfl){
    mesh joint  = mesh_primitive_sphere(r0);
    for(size_t k=0; k< joint.position.size(); ++k)
    {
        if(joint.position[k].y >= 0){
            joint.position[k].x *= 1 + 0.3f*(halfl - joint.position[k].y);
            joint.position[k].z *= 1 + 0.1f*(halfl-joint.position[k].y);
            joint.position[k].y *= 2.0f;
        }
        else{
            joint.position[k].x *= 1 + 0.3f*(halfl + joint.position[k].y);
            joint.position[k].z *= 1 + 0.1f*(halfl + joint.position[k].y);    
        }
        
    }
    return joint;
}

mesh superior_leg(const float r0, const float l){
    mesh leg = mesh_primitive_cylinder(r0,{0,-l/2,0},{0,0,0});
    //y comeca 0 e fica negativo   
    for(size_t k=0; k< leg.position.size(); ++k)
    {
        leg.position[k].x *= 1 + 0.3f*(leg.position[k].y+ l/2);
        leg.position[k].z *= 1 + 0.1f*(leg.position[k].y + l/2);
        //zebra_body.position[k].y *= 0.5f;
    }

    return leg;
}

mesh horse_body(const float r){

    mesh body = mesh_primitive_sphere(r);
    
    for(size_t k=0; k< body.position.size(); ++k)
    {
        body.position[k].x *= 1.5f;
        body.position[k].z *= 0.9f;
        body.position[k].y *= 0.9f;
    }
    
    for(size_t k=0; k< body.position.size(); ++k)
    {
        if( body.position[k].x < -1.42f*r){
            body.position[k].y *= (1 - (1.42f*r + body.position[k].x));
            body.position[k].z *= (1 - (1.42f*r + body.position[k].x));
        }
    }

    return body;
}


mesh create_cone_trunk(float r_top, float r_bottom, float height, vec3 translation) {
	const unsigned int N = 20;
	const unsigned int N2 = 20;

	mesh cylinder;
	cylinder.position.resize(N2*N);

	// Fill cylinder geometry
	for (size_t i=0; i<N2; i++) {
		for (size_t ku=0; ku<N; ++ku) {
			vec3 center = {0, i*height/(N2-1), 0};
			const float r = r_bottom + i*(r_top - r_bottom)/(N2-1);
			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = sin(ku /(1.0*N) * 2*M_PI );
			const float v = cos(ku /(1.0*N) * 2*M_PI );
			vec3 offset = {r * u, 0, r * v};
			// Compute coordinates
			cylinder.position[i*N + ku] = translation + center + offset;
		}
	}

	// Generate triangle organization
	for (unsigned int i=0; i<N2-1; i++) {
		for (unsigned int ku=0; ku<N-1; ++ku) {
			index3 triangle  = {i*N + ku, i*N + ku + 1,(i+1)*N + ku};
			index3 triangle2 = {(i+1)*N + ku + 1, (i+1)*N + ku, i*N + ku + 1,};
			cylinder.connectivity.push_back(triangle);
			cylinder.connectivity.push_back(triangle2);
		}
		index3 triangle  = {i*N + N-1, i*N   ,  (i+1)*N + N-1 };
		index3 triangle2 = {(i+1)*N  ,          (i+1)*N + N-1 , i*N};
		cylinder.connectivity.push_back(triangle);
		cylinder.connectivity.push_back(triangle2);
	}
	return cylinder;
}


mesh zebra_neck(const float r_neck, const float x_neck){
    
    const float tg70 =  2.75f;

    mesh neck = mesh_primitive_cylinder(r_neck,{0,0,0}, {x_neck, tg70*x_neck,0});
    for(size_t k =0 ; k < neck.position.size(); k++){
        neck.position[k].x *= exp((1.0f -neck.position[k].y/(tg70*x_neck))/5.0f);
        neck.position[k].z *= exp((1.0f -neck.position[k].y/(tg70*x_neck))/5.0f);
    }

    return neck;
}


mesh zebra_ear(const float r_neck){
    const float r_ear = r_neck/7.0f;
    const float h = 2.0f*1.6180f * r_ear;
    mesh ear = mesh_primitive_cone(r_ear, {0,0,0}, {-h, 0 ,0});
    for(size_t k = 0; k < ear.position.size(); k++){
        ear.position[k].y *= 0.4f;
        ear.position[k].z *= 1.1f;
    }

    return ear;
}
