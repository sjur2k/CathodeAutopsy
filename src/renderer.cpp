#include "renderer.hpp"

Renderer::Renderer(const std::vector<glm::vec3>& vertices, GLenum draw_mode)
    : vertex_count_(vertices.size()), draw_mode_(draw_mode) {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
}

void Renderer::draw(
    Shader& shader, 
    const glm::mat4& projection, 
    const glm::mat4& view,
    const glm::mat4& model
){
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);
    
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(VAO_);
    glDrawArrays(draw_mode_, 0, static_cast<GLsizei>(vertex_count_));
    glBindVertexArray(0);
}