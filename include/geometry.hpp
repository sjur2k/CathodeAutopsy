#pragma once

struct Position {
     float x;
     float y;
     float z;

     Position()
         : x(0.0f), y(0.0f), z(0.0f) {}

     Position(float x_, float y_, float z_)
         : x(x_), y(y_), z(z_) {}
};

struct Rotation {
     float roll;
     float pitch;
     float yaw;

     Rotation()
         : roll(0.0f), pitch(0.0f), yaw(0.0f) {}

     Rotation(float roll_, float pitch_, float yaw_)
         : roll(roll_), pitch(pitch_), yaw(yaw_) {}
};

struct Pose {
     Position position;
     Rotation rotation;

     Pose()
         : position(), rotation() {}

     Pose(const Position& position_, const Rotation& rotation_)
         : position(position_), rotation(rotation_) {}
};

