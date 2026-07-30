#include "rendering/texture.hpp"
#include "core/paths.hpp"

#include "stb_image.h"
#include <glad.h>

#include <stdexcept>
#include <string>

Texture::Texture(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);

    std::string full_path = paths::asset(path).string();
    unsigned char* data = stbi_load(full_path.c_str(), &width_, &height_, &channels_, 0);
    if (!data){
        throw std::runtime_error(
            "Failed to load texture: " + full_path + " (" + stbi_failure_reason() + ")"
        );
    }
    GLenum format = (channels_ == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &id_);
}

// Core behavior
void Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id_);
}
