#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader {
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        
        // Core behavior
        void use() const {glUseProgram(id_);}

        // Uniform setters
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec4(const std::string &name, const glm::vec4 &value) const;
    
    private:
        std::string readFile(const char* path) const;
        void checkCompileErrors(unsigned int shader, const std::string& type) const;

        unsigned int id_ = 0;
};