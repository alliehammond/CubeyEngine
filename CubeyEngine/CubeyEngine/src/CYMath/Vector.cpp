#include "EnginePCH.h"
#include "CYMath\Vector.h"

namespace CBY
{
    Vector Vector::operator+(const Vector &lhs)
    {
        Vector newVec = *this;
        newVec += lhs;

        return newVec;
    }

    Vector& Vector::operator+=(const Vector& lhs)
    {
        x += lhs.x;
        y += lhs.y;
        z += lhs.z;

        return *this;
    }

    Vector Vector::operator-(const Vector& lhs)
    {
        Vector newVec = *this;
        newVec -= lhs;

        return newVec;
    }

    Vector& Vector::operator-=(const Vector& lhs)
    {
        x -= lhs.x;
        y -= lhs.y;
        z -= lhs.z;

        return *this;
    }

    Vector Vector::operator*(const float lhs)
    {
        Vector newVec = *this;
        newVec *= lhs;

        return newVec;
    }

    Vector& Vector::operator*=(const float lhs)
    {
        x *= lhs;
        y *= lhs;
        z *= lhs;

        return *this;
    }

    Vector Vector::operator/(const float lhs)
    {
        Vector newVec = *this;
        newVec /= lhs;

        return newVec;
    }

    Vector& Vector::operator/=(const float lhs)
    {
        x /= lhs;
        y /= lhs;
        z /= lhs;

        return *this;
    }
};
