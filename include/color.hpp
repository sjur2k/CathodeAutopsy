#pragma once

#include "glm/glm.hpp"

struct Color{
    float r,g,b;
    float opacity = 1.0f;
    constexpr operator glm::vec3() const noexcept {
        return glm::vec3(r,g,b);
    }
    constexpr operator glm::vec4() const noexcept {
        return glm::vec4(r,g,b,opacity);
    }
};

namespace Colors{
    // Solid colors (opacity defaults to 100%)
    inline constexpr Color Floor       = { 0.20f, 0.30f, 0.40f}; //lighter gray/blue
    inline constexpr Color Background  = { 0.10f, 0.15f, 0.20f}; //darker  gray/blue
    inline constexpr Color Red         = { 1.00f, 0.00f, 0.00f};
    inline constexpr Color Green       = { 0.00f, 1.00f, 0.00f};
    inline constexpr Color Blue        = { 0.00f, 0.00f, 1.00f};
    inline constexpr Color White       = { 1.00f, 1.00f, 1.00f};
    inline constexpr Color Black       = { 0.00f, 0.00f, 0.00f};
    inline constexpr Color Cyan        = { 0.00f, 1.00f, 1.00f};
    inline constexpr Color Magenta     = { 1.00f, 0.00f, 1.00f};
    inline constexpr Color Yellow      = { 1.00f, 1.00f, 0.00f};

    // Opaque colors
    inline constexpr Color WhiteHalfOpaque    = { 1.00f, 1.00f, 1.00f, 0.50f};
    inline constexpr Color WhiteQuarterOpaque = { 1.00f, 1.00f, 1.00f, 0.25f};
}

