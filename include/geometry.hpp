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

struct IntVector {
    int a,b,c;
};

struct Position {
    float x,y,z;
    using is_3D_vector = void; // allows operator overloads

    Position()
        : x(0.0f), y(0.0f), z(0.0f) {}

    Position(float x_, float y_, float z_)
        : x(x_), y(y_), z(z_) {}

    Position operator+(const Position& other) const {
        return Position{x + other.x, y + other.y, z + other.z};
    }
    IntVector to_int() const{
        return {
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(z)
        };
    }
};
struct Rotation {
    float pitch, yaw, roll;
    using is_3D_vector = void; // allows operator overloads

    Rotation()
        : pitch(0.0f), yaw(0.0f), roll(0.0f) {}

    Rotation(float pitch_, float yaw_, float roll_)
        : pitch(pitch_), yaw(yaw_), roll(roll_) {}

    Rotation operator+(const Rotation& other) const {
        return Rotation{pitch + other.pitch, yaw + other.yaw, roll + other.roll};
    }
    IntVector to_int() const{
        return {
            static_cast<int>(pitch),
            static_cast<int>(yaw),
            static_cast<int>(roll)
        };
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

