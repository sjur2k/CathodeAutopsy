#pragma once

template<typename T>
concept MathStruct3D = requires {
    typename T::is_3D_vector; // To ensure following templates only apply to the tagged structs.
};

template<typename MathStruct3D>
MathStruct3D operator+(const MathStruct3D& lhs, const MathStruct3D& rhs){
    auto [a,b,c] = lhs;
    auto [d,e,f] = rhs;
    return MathStruct3D{a+d,b+e,c+f};
}
template<typename MathStruct3D>
MathStruct3D operator-(const MathStruct3D& lhs, const MathStruct3D& rhs){
    auto [a,b,c] = lhs;
    auto [d,e,f] = rhs;
    return MathStruct3D{a-d,b-e,c-f};
}
template<typename MathStruct3D>
MathStruct3D operator*(const MathStruct3D& lhs, const auto rhs){
    auto [a,b,c] = lhs;
    return MathStruct3D(a*rhs, b*rhs, c*rhs); // to allow implicit truncating back to integer if needed.
}
template<typename MathStruct3D>
MathStruct3D operator/(const MathStruct3D& lhs, const auto rhs){
    auto [a,b,c] = lhs;
    return MathStruct3D(a/rhs, b/rhs, c/rhs);
}
template<typename MathStruct3D>
MathStruct3D& operator+=(MathStruct3D& lhs, const MathStruct3D& rhs){
    auto& [a,b,c] = lhs;
    auto [d,e,f] = rhs;
    a+=d; b+=e; c+=f;
    return lhs;
}
template<typename MathStruct3D>
MathStruct3D& operator-=(MathStruct3D& lhs, const MathStruct3D& rhs){
    auto& [a,b,c] = lhs;
    auto [d,e,f] = rhs;
    a-=d; b-=e; c-=f;
    return lhs;
}
template<typename MathStruct3D>
MathStruct3D& operator*=(MathStruct3D& lhs, const auto rhs){
    auto& [a,b,c] = lhs;
    a*=rhs; b*=rhs; c*=rhs;
    return lhs;
}
template<typename MathStruct3D>
MathStruct3D& operator/=(MathStruct3D& lhs, const auto rhs){
    auto& [a,b,c] = lhs;
    a/=rhs; b/=rhs; c/=rhs;
    return lhs;
}

struct Position {
    float x;
    float y;
    float z;
    using is_3D_vector = void; // allows operator overloads

    Position()
        : x(0.0f), y(0.0f), z(0.0f) {}

    Position(float x_, float y_, float z_)
        : x(x_), y(y_), z(z_) {}

    Position operator+(const Position& other) const {
        return Position{x + other.x, y + other.y, z + other.z};
    }
};
struct Rotation {
    float roll;
    float pitch;
    float yaw;
    using is_3D_vector = void; // allows operator overloads

    Rotation()
        : roll(0.0f), pitch(0.0f), yaw(0.0f) {}

    Rotation(float roll_, float pitch_, float yaw_)
        : roll(roll_), pitch(pitch_), yaw(yaw_) {}

    Rotation operator+(const Rotation& other) const {
        return Rotation{roll + other.roll, pitch + other.pitch, yaw + other.yaw};
    }
};

struct Pose {
    Position position;
    Rotation rotation;

    Pose()
        : position(), rotation() {}

    Pose(const Position& position_, const Rotation& rotation_)
        : position(position_), rotation(rotation_) {}
    
    Pose operator+(const Pose& other) const {
        return Pose{position + other.position, rotation + other.rotation};
    }
    Pose operator-(const Pose& other) const {
        return Pose{position - other.position, rotation - other.rotation};
    }
    Pose& operator+=(const Pose& other){
        this->position += other.position;
        this->rotation += other.rotation;
        return *this;
    }
    Pose& operator-=(const Pose& other){
        this->position -= other.position;
        this->rotation -= other.rotation;
        return *this;
    }
    // Defining a multiplication operator does not make sense here, 
    // as the two structs are conceptually different and do not scale the same way
};

