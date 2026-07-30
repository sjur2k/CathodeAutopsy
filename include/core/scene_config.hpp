#pragma once

#include "transform.hpp"

namespace SceneConfig {
    // Grid (SI units)
    inline constexpr float kCellWidth = 10.0f;
    inline constexpr float kCellLength = 4.0f;
    inline constexpr float kResolution = 0.05f;

    // Camera
    inline constexpr float kFieldOfView = 90.0f;
    inline constexpr float kNearPlane = 0.1f;
    inline constexpr float kFarPlane = 300.0f;

    inline const Pose kInitialCameraPose{
        Position(0.0f, 70.0f, 105.0f),
        Rotation(-60.0f, 90.0f, 0.0f)
    }; // Looks down on grid defined in x-z plane
}