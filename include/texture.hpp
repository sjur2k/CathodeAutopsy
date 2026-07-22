#pragma once
#include <string>

class Texture{
    public:
        explicit Texture(const std::string& path);
        ~Texture();
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void bind(unsigned int unit = 0) const;
        int width() const {return width_;}
        int height() const {return height_;}

    private:
        unsigned int id_ = 0;
        int width_ = 0, height_ = 0, channels_ = 0;
};