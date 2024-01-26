#pragma once
#include <CommonUtilities/MathBundle.hpp>
enum class ShapeType
{
    None,

    Sphere,
    Cone
};

struct Shape
{
    ShapeType type;
#pragma warning(disable:4201)
    union // Shapes
    {
        struct Sphere
        {
            float radius;
        } asSphere;
        struct Cone
        {
            float radius;
            float angle;
            float x;
            float y;
        } asCone;
    };
#pragma warning(default:4201)
};
