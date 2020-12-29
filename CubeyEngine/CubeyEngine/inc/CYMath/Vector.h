#pragma once

namespace CBY
{
    class Vector
    {
    public:
        Vector() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
        float x, y, z;
    };
};
