#pragma once

namespace CBY
{
    class Vector
    {
    public:
        Vector() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
        Vector operator+(const Vector &lhs);
        Vector& operator+=(const Vector &lhs);
        Vector operator-(const Vector& lhs);
        Vector& operator-=(const Vector& lhs);
        //Performs component-wise multiplication/division
        Vector operator*(const float lhs);
        Vector& operator*=(const float lhs);
        Vector operator/(const float lhs);
        Vector& operator/=(const float lhs);
        
        void Normalize();

        float x, y, z;
    };
};
