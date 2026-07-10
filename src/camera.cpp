#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "geometry.hpp"
#include "grid.hpp"
#include "camera.hpp"

Pose Camera::get_pose() const {
    return pose_;
}

void Camera::set_pose(const Pose& new_pose) {
    pose_ = new_pose;
}

void Camera::reset_pose(){
    pose_ = initial_pose_;
}

glm::mat4 Camera::get_view_matrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::rotate(view, -glm::radians(pose_.rotation.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(pose_.rotation.yaw),   glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(-pose_.position.x, -pose_.position.y, -pose_.position.z));
    return view;
}

glm::mat4 Camera::get_projection_matrix() const {
    float fov_rad = glm::radians(fov_);
    float tan_half_fov = tan(fov_rad / 2.0f);
    glm::mat4 projection = glm::mat4(0.0f);
    projection[0][0] = 1.0f / (aspect_ratio_ * tan_half_fov);
    projection[1][1] = 1.0f / (tan_half_fov);
    projection[2][2] = -(far_plane_ + near_plane_) / (far_plane_ - near_plane_);
    projection[2][3] = -1.0f;
    projection[3][2] = -(2.0f * far_plane_ * near_plane_) / (far_plane_ - near_plane_);
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
    auto [x,y,z] = pose_.position.to_int();
    auto [pitch, yaw, roll] = pose_.rotation.to_int();
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