#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "geometry.hpp"
#include "grid.hpp"
#include "camera.hpp"

Pose Camera::get_pose() const {
    return pose;
}

void Camera::set_pose(const Pose& new_pose) {
    pose = new_pose;
}

void Camera::reset_pose(){
    pose = initial_pose;
}

glm::mat4 Camera::get_view_matrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::rotate(view, -glm::radians(pose.rotation.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(pose.rotation.yaw),   glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(-pose.position.x, -pose.position.y, -pose.position.z));
    return view;
}

glm::mat4 Camera::get_projection_matrix() const {
    float fov_rad = glm::radians(fov);
    float tan_half_fov = tan(fov_rad / 2.0f);
    glm::mat4 projection = glm::mat4(0.0f);
    projection[0][0] = 1.0f / (aspect_ratio * tan_half_fov);
    projection[1][1] = 1.0f / (tan_half_fov);
    projection[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    projection[2][3] = -1.0f;
    projection[3][2] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    return projection;
}
const char* space(auto num){
    int num_spaces = 0;
    num >= 0 ? void(num_spaces++) : void();
    if (abs(num)<=10){
        num_spaces+=2;
    } else if (abs(num)<=100){
        num_spaces++;
    }
    std::string spaces;
    for (int i = 0; i < num_spaces; i++){
        spaces += " ";
    } 
    return spaces.c_str();
}

void Camera::print_pose(){
    bool sign = false;
    auto [x,y,z] = pose.position.to_int();
    auto [pitch, yaw, roll] = pose.rotation.to_int();
    std::cout
        << "Current position:    ("
        << space(x) << x << ","
        << space(y) << y << ","
        << space(z) << z << ")\n"
        << "Current orientation: ("
        << space(pitch) << pitch << ","
        << space(yaw)   << yaw   << ","
        << space(roll)  << roll  << ")\n\n";
}