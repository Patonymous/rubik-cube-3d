#ifndef RAYLIB_VERSION_HPP_INCLUDED
#define RAYLIB_VERSION_HPP_INCLUDED

#ifndef USE_SFML

#include "main.hpp"
#include <raylib.h>

struct CameraProperties
{
    Camera3D cam;
    Vector2 deg;
    Vector3 rad;
};

struct Animation
{
    enum Type {None, Pitch, Yaw, Roll} type;
    Tile where;
    float angle;
    Animation(Type t, Tile w, float a):type(t),where(w),angle(a){}
    Animation():Animation(None,Tile::invalid,0){}
};

void RAYLIB_IO(Cube& cube, Tile& mouse);

#endif // USE_SFML

#endif // RAYLIB_VERSION_HPP_INCLUDED
