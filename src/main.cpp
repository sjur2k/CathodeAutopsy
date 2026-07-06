#include <stdio.h>
#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "grid.hpp"
#include "camera.hpp"
#include "shader.hpp"

int main(){
    bool verbose = false;
    //Assumed constants (in SI-units):
    float cell_width = 10.0f; //Corresponds to rows (y)
    float cell_length = 4.0f; //Corresponds to columns (x)
    float resolution = 0.05f; // Assumed resolution of point cloud
    Grid grid(cell_width/resolution, cell_length/resolution);
    grid.fill_random_smooth();
    
    verbose ? grid.print_grid() : void();
    grid.write_grid_to_PPM();
    grid.write_grid_to_csv(resolution);

    //grid.print_grid();
    //grid.write_grid_to_PPM();
    //grid.write_grid_to_csv(resolution);
    std::vector<glm::vec3> grid_data = grid.get_point_cloud_vec3();
    Pose initial_camera_pose = Pose(Position(50.0f, 100.0f, 100.0f), Rotation(0.0f, 0.0f, 0.0f));
    float field_of_view = 90.0f;
    float aspect_ratio = 1.0f;
    float near_plane = 0.1f;
    float far_plane = 300.0f;
    Camera camera(initial_camera_pose, field_of_view, aspect_ratio, near_plane, far_plane);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Cathode Visualization", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, grid_data.size() * sizeof(glm::vec3), grid_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window)){
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, true);
        }
        Pose current_pose = camera.get_pose();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            current_pose.position.z -= 0.1f; // Move the camera along the z-axis
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            current_pose.position.z += 0.1f; // Move the camera along the z-axis
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            current_pose.position.x -= 0.1f; // Move the camera along the x-axis
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            current_pose.position.x += 0.1f; // Move the camera along the x-axis
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            current_pose.position.y += 0.1f; // Move the camera along the y-axis
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            current_pose.position.y -= 0.1f; // Move the camera along the y-axis
        }
        camera.set_pose(current_pose);
       /*  while (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
            poll_mouse();
        } */
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glEnable(GL_PROGRAM_POINT_SIZE);
    
        shader.use();
        shader.setMat4("view", camera.get_view_matrix());
        shader.setMat4("projection", camera.get_projection_matrix());
        shader.setMat4("model", glm::mat4(1.0f)); //Plot as given
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(grid_data.size()));
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}