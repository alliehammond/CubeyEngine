#pragma once

namespace CBY
{
    class Vector4
    {
    public:
        Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

        float x, y, z, w;
    };
};
