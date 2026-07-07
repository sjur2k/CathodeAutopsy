/* #include <stdio.h>
#include <cmath>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "grid.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "renderer.hpp"
#include "input_manager.hpp"
#include "window.hpp" 

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

    Window window(1920,1080, "Cathode Visualization");
    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    Renderer renderer(grid_data);
    InputManager input_manager(window.get_handle(), camera);

    float last_frame_time = 0.0f;

    while(!window.should_close()){
        if (glfwGetKey(window.get_handle(), GLFW_KEY_Q) == GLFW_PRESS){
            glfwSetWindowShouldClose(window.get_handle(), true);
        }

        float current_time = static_cast<float>(glfwGetTime());
        float delta_time = current_time - last_frame_time;
        last_frame_time = current_time;
    
        input_manager.process_input(delta_time);

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        renderer.draw(shader, camera.get_projection_matrix(), camera.get_view_matrix(), glm::mat4(1.0f));
        window.swap_buffers();
        glfwPollEvents(); 
    }
    return 0;
}*/

#include "application.hpp"

int main(){
    bool verbose = false;
    Application app(verbose);
    app.run();
    return 0;
}