#include <iostream>
#include <string>
#include "application.hpp"
#include "mesh.hpp"
#include "color.hpp"
#include "renderer.hpp"
#include "geometry.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {
    constexpr int kWindowWidth = 1920;
    constexpr int kWindowHeight = 1080;

    // Assumed constants (SI-units)
    constexpr float kCellWidth = 10.0f;
    constexpr float kCellLength = 4.0f;
    constexpr float kResolution = 0.05f;

    // Constants for rendering
    constexpr float kGridWidth = kCellWidth / kResolution;
    constexpr float kGridLength = kCellLength / kResolution;
    constexpr float kPadding = 10.0f;

    // Camera parameters
    constexpr float kFieldOfView = 90.0f;
    constexpr float kNearPlane = 0.1f;
    constexpr float kFarPlane = 300.0f;
}

static std::vector<glm::vec3> build_point_cloud(Grid& grid){
    grid.fill_random_smooth();
    return grid.get_point_cloud_vec3();
}

static std::vector<glm::vec3> build_unit_quad(){
    return {
        {-0.5f, -0.5f, 0.0f}, { 0.5f, -0.5f, 0.0f}, { 0.5f,  0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f}, { 0.5f,  0.5f, 0.0f}, {-0.5f,  0.5f, 0.0f},
    };
}

static glm::mat4 build_floor_model(){
    glm::mat4 model = glm::translate(
        glm::mat4(1.0f), 
        glm::vec3(kGridLength/2.0f, 0.0f, kGridWidth/2.0f)
    );
    model = glm::rotate(
        model, 
        glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f)
    );
    model = glm::scale(
        model,
        glm::vec3(kGridLength + 2*kPadding, kGridWidth + 2*kPadding, 1.0f)
    );
    return model;
}

static glm::mat4 build_ui_quad_model(float x, float y, float width, float height){
    glm::mat4 model = glm::translate(glm::mat4(1.0f),
        glm::vec3(x + width / 2.0f, y + height / 2.0f, 0.0f)
    );
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    return model;
}

static glm::mat4 build_ui_quad_model(TextBlockLayout block){
    return build_ui_quad_model(block.box_x, block.box_y, block.box_width, block.box_height);
}

Application::Application() : 
    window_(kWindowWidth, kWindowHeight, "Cathode Visualization"), 
    camera_(
        Pose(Position(0.0f, 70.0f, 105.0f), Rotation(-60.0f, 90.0f, 0.0f)), // Looks down on grid defined in x-z plane
        kFieldOfView,
        static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        kNearPlane,
        kFarPlane),
    shader_("shaders/basic.vert", "shaders/basic.frag"),
    grid_(kCellWidth / kResolution, kCellLength / kResolution),
    point_cloud_renderer_(build_point_cloud(grid_), GL_POINTS),
    quad_renderer_(build_unit_quad(), GL_TRIANGLES),
    display_text_renderer_("fonts/IvarDisplayHydro-Regular.ttf", 96, kWindowWidth, kWindowHeight),
    regular_text_renderer_("fonts/IvarTextHydro-Regular.ttf", 48, kWindowWidth, kWindowHeight),
    input_manager_(window_.get_handle(), camera_)
{
    glEnable(GL_DEPTH_TEST);
    ui_projection_ = glm::ortho(0.0f, static_cast<float>(kWindowWidth), 
                                0.0f, static_cast<float>(kWindowHeight));
}

void Application::run() {
    bool was_paused = false;
    int last_fb_width = 0, last_fb_height = 0;
    glfwGetFramebufferSize(window_.get_handle(), &last_fb_width, &last_fb_height);
    last_camera_pose_ = camera_.get_pose();

    while (!window_.should_close()) {
        bool paused_now = input_manager_.is_paused();
        if (paused_now){
            glfwWaitEvents();
        } else {
            glfwPollEvents();
        }
        
        paused_now = input_manager_.is_paused();
        if (was_paused != paused_now){
            needs_redraw_ = true;
            if (was_paused && !paused_now){
                last_frame_time_ = static_cast<float>(glfwGetTime());
            }    
        }
        was_paused = paused_now;

        int fb_width, fb_height;
        glfwGetFramebufferSize(window_.get_handle(), &fb_width, &fb_height);
        if (fb_width != last_fb_width || fb_height != last_fb_height){
            last_fb_width = fb_width;
            last_fb_height = fb_height;
            needs_redraw_ = true;
        }

        float current_time = static_cast<float>(glfwGetTime());
        float delta_time = current_time - last_frame_time_;
        last_frame_time_ = current_time;

        if(!paused_now){
            update(delta_time);
            
            Pose current_pose = camera_.get_pose();
            if (current_pose != last_camera_pose_){
                needs_redraw_ = true,
                last_camera_pose_ = current_pose;
            }
        }
        if(needs_redraw_){ // Assumes constant grid, no moving objects etc..
            render();
            window_.swap_buffers();
            needs_redraw_ = false;
        }
    }
}

void Application::draw_scene(){
    auto [r,g,b,opacity] = Colors::Background;
    glClearColor(r,g,b,opacity);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 proj = camera_.get_projection_matrix();
    glm::mat4 view = camera_.get_view_matrix();

    shader_.setVec4("color", Colors::Cyan);
    point_cloud_renderer_.draw(shader_, proj, view, glm::mat4(1.0f));
    
    shader_.setVec4("color", Colors::Floor);
    quad_renderer_.draw(shader_, proj, view, build_floor_model());
}

void Application::draw_hud(){
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Color text_color = Colors::Background;
    float textbox_padding = 10.0f;

    std::string msg = "For information, press ESC";
    float text_scale = 0.4f;

    glm::vec2 text_dims = regular_text_renderer_.measure_text(msg, text_scale);
    glm::vec2 text_coords(textbox_padding, kWindowHeight - text_dims.y - textbox_padding);
    TextLine text(msg, text_coords, text_scale, text_color);

    glm::mat4 textbox_model = build_ui_quad_model(
        0.0f,
        kWindowHeight - text_dims.y - 2.0f * textbox_padding,
        text_dims.x + 2 * textbox_padding,
        text_dims.y + 2 * textbox_padding
    );

    shader_.setVec4("color", Colors::Background);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), textbox_model);
    shader_.setVec4("color", Colors::WhiteOpaque);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), textbox_model);
    regular_text_renderer_.render_text(text);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Application::draw_pause_overlay(){
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Color text_color = Colors::White;
    float textbox_padding = 20.0f;
    float lineshift = 30.0f;
    
    TextBlockLayout top_block = build_text_block(
        {
            TextLine("PAUSED", 0.0f, 0.0f, 1.0f, text_color, &display_text_renderer_),
            TextLine("Press ESC to resume", 0.0f, 0.0f, 0.8f, text_color, &regular_text_renderer_),
        },
        kWindowWidth * 0.5f, kWindowHeight * 0.85f,
        lineshift, textbox_padding
    );

    TextBlockLayout bottom_block = build_text_block(
        {
            TextLine("Controls", 0.0f, 0.0f, 0.8f, text_color, &display_text_renderer_),
            TextLine("W/A/S/D: Horizontal Movement", 0.0f, 0.0f, 0.6f, text_color, &regular_text_renderer_),
            TextLine("LSHIFT/SPACE: Vertical Movement", 0.0f, 0.0f, 0.6f, text_color, &regular_text_renderer_),
            TextLine("MOUSE1 (HOLD): Pan", 0.0f, 0.0f, 0.6f, text_color, &regular_text_renderer_)
        },
        kWindowWidth * 0.5f, kWindowHeight * 0.5f,
        lineshift, textbox_padding
    );

    shader_.setVec4("color", Colors::WhiteOpaque);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), 
        build_ui_quad_model(0.0f,0.0f,kWindowWidth, kWindowHeight)
    );
    
    shader_.setVec4("color", Colors::Background);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), 
        build_ui_quad_model(top_block)
    );
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), 
        build_ui_quad_model(bottom_block)
    );

    for (auto& text : top_block.texts){
        text.renderer->render_text(text);
    }
    for (auto& text : bottom_block.texts){
        text.renderer->render_text(text);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Application::update(float delta_time) {
    input_manager_.process_input(delta_time);
}

void Application::render() {
    draw_scene();
    if (!input_manager_.is_paused()) {
        draw_hud();
    } else {
        draw_pause_overlay();
    }       
}