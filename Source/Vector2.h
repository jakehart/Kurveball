// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <istream>

#include "MathUtils.h"

namespace CurveLib
{
    template<typename ScalarT>
    class Vector2;

    using Float2 = Vector2<float>;
    using Double2 = Vector2<double>;

    template<typename ScalarT>
    class Vector2
    {
    public:
        using ScalarType = ScalarT;

        ScalarT X {};
        ScalarT Y {};

        [[nodiscard]] static Vector2 Lerp(const Vector2& first, const Vector2& second, ScalarT lerpT)
        {
            return Lerp2D(first, second, lerpT);
        }

        std::string ToString() const
        {
            std::stringstream ss;
            ss << "(" << X << ", " << Y << ")";
            return ss.str();
        }

        void ToBinary(std::ostream& outStream) const
        {
            outStream.write((const char*) &X, sizeof(ScalarType));
            outStream.write((const char*) &Y, sizeof(ScalarType));
            
            outStream.flush();
        }

        [[nodiscard]] static Vector2 FromBinary(std::istream& istream)
        {
            ScalarT x = 0;
            ScalarT y = 0;

            istream.read((char*) &x, sizeof(ScalarType));
            istream.read((char*) &y, sizeof(ScalarType));

            return Vector2(x, y);
        }
    };

    template<typename ScalarT>
    Vector2<ScalarT> operator*(const Vector2<ScalarT>& vector, ScalarT scalar)
    {
        return Vector2(vector.X * scalar, vector.Y * scalar);
    }

    template<typename ScalarT>
    Vector2<ScalarT> operator+(const Vector2<ScalarT>& lhs, const Vector2<ScalarT>& rhs)
    {
        return Vector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
    }
}