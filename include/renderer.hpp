#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.hpp"

struct TexturedVertex {
    glm::vec3 pos; 
    glm::vec2 uv;
};

class Renderer {
    public:
        Renderer(
            const std::vector<glm::vec3>& vertices, 
            GLenum draw_mode = GL_POINTS
        );
        Renderer(
            const std::vector<TexturedVertex>& vertices, 
            GLenum draw_mode = GL_TRIANGLES
        );
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void draw(
            Shader& shader, 
            const glm::mat4& projection, 
            const glm::mat4& view,
            const glm::mat4& model
        );

        void update_vertices(const std::vector<glm::vec3>& vertices);
    private:
        unsigned int VAO_ = 0;
        unsigned int VBO_ = 0;
        size_t vertex_count_ = 0;
        GLenum draw_mode_;
};