#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "grid.hpp"


namespace Mesh {
    std::vector<glm::vec3> terrain(const Grid& grid);
    std::vector<glm::vec3> floor(
        float x_min, float x_max, float z_min, float z_max, float y
    );
    std::vector<glm::vec3> grid_lines(
        float x_min, float x_max, float z_min, float z_max, float y, int divisions
    );
}