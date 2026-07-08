#include "mesh.hpp"

namespace Mesh{
std::vector<glm::vec3> terrain(const Grid& grid){
    std::vector<glm::vec3> vertices;
    auto [rows,cols] = grid.get_dimensions();
    vertices.reserve((rows - 1)*(cols - 1)*6);
    for (int j = 0; j < rows - 1; j++) {
        for (int i = 0; i < cols - 1; i++) {
            glm::vec3 p00(i,     grid.get_value(i,   j  ), j    );
            glm::vec3 p10(i + 1, grid.get_value(i+1, j  ), j    );
            glm::vec3 p01(i,     grid.get_value(i,   j+1), j + 1);
            glm::vec3 p11(i + 1, grid.get_value(i+1, j+1), j + 1);

            vertices.push_back(p00);
            vertices.push_back(p10);
            vertices.push_back(p11);

            vertices.push_back(p00);
            vertices.push_back(p11);
            vertices.push_back(p01);
        }
    }
    return vertices;
}

std::vector<glm::vec3> floor(
    float x_min, float x_max,
    float z_min, float z_max,
    float y)
{
    return {
        {x_min, y, z_min},
        {x_max, y, z_min},
        {x_max, y, z_max},

        {x_min, y, z_min},
        {x_max, y, z_max},
        {x_min, y, z_max},
    };
}

std::vector<glm::vec3> grid_lines(
    float x_min, float x_max,
    float z_min, float z_max,
    float y,
    int divisions)
    {
        std::vector<glm::vec3> vertices;
        vertices.reserve((divisions + 1) * 4);

        float x_step = (x_max - x_min) / divisions;
        float z_step = (z_max - z_min) / divisions;

        // X-axis
        for (int j = 0; j <= divisions; j++) {
            float z = z_min + j * z_step;
            vertices.emplace_back(x_min, y, z);
            vertices.emplace_back(x_max, y, z);
        }

        // Z-axis
        for (int i = 0; i <= divisions; i++) {
            float x = x_min + i * x_step;
            vertices.emplace_back(x, y, z_min);
            vertices.emplace_back(x, y, z_max);
        }

        return vertices;
    }

}