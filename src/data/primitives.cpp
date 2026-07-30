#include "data/primitives.hpp"

namespace Primitives {

std::vector<glm::vec3> unit_quad(){
    return {
        {-0.5f, -0.5f, 0.0f}, { 0.5f, -0.5f, 0.0f}, { 0.5f,  0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f}, { 0.5f,  0.5f, 0.0f}, {-0.5f,  0.5f, 0.0f},
    };
}

std::vector<TexturedVertex> textured_unit_quad(){
    return {
        {{-0.5f,-0.5f,0.0f},{0.0f,0.0f}}, {{0.5f,-0.5f,0.0f},{1.0f,0.0f}}, 
        {{0.5f,0.5f,0.0f},{1.0f,1.0f}}, {{-0.5f,-0.5f,0.0f},{0.0f,0.0f}}, 
        {{0.5f,0.5f,0.0f},{1.0f,1.0f}}, {{-0.5f,0.5f,0.0f},{0.0f,1.0f}}
    };
}

}