#pragma once

#include "rendering/renderer.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Primitives {
    std::vector<glm::vec3> unit_quad();
    std::vector<TexturedVertex> textured_unit_quad();
}