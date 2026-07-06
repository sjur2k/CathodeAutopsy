#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.hpp"

class Renderer {
    public:
        explicit Renderer(const std::vector<glm::vec3>& points);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void draw(
            Shader& shader, 
            const glm::mat4& projection, 
            const glm::mat4& view,
            const glm::mat4& model
        );
    private:
        unsigned int VAO_ = 0;
        unsigned int VBO_ = 0;
        size_t num_points_ = 0;
};