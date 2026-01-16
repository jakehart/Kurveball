// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <string>

namespace CurveLib
{
    template<typename ScalarType>
    class Vector3;

    using Float3 = Vector3<float>;
    using Velocity = Float3;
    using Position = Float3;
    using EulerAngles = Float3;

    // Duck-typed constructor that can initialize from any type that has numeric members called X, Y, and Z.
    template<typename ExternalVectorT>
    Float3 ConvertFloat3(const ExternalVectorT& otherVector);

    // Vector * float scalar
    template<typename ScalarType>
    Vector3<ScalarType> operator*(const Vector3<ScalarType>& vector, ScalarType scalar);

    // Vector * vector
    template<typename FirstScalarType, typename SecondScalarType>
    Vector3<FirstScalarType> operator*(const Vector3<FirstScalarType>& lhs, const Vector3<SecondScalarType>& rhs);

    // Vector / scalar
    template<typename ScalarType>
    Vector3<ScalarType> operator/(const Vector3<ScalarType>& vector, ScalarType scalar);

    template<typename ScalarType>
    Vector3<ScalarType> operator+(const Vector3<ScalarType>& lhs, const Vector3<ScalarType>& rhs);

    template<typename ScalarType>
    Vector3<ScalarType> operator-(const Vector3<ScalarType>& lhs, const Vector3<ScalarType>& rhs);
    
    template<typename ScalarType>
    class Vector3
    {
    public:
        using Scalar = ScalarType;
        Vector3();
        Vector3(ScalarType x, ScalarType y, ScalarType z);

        ScalarType X {};
        ScalarType Y {};
        ScalarType Z {};

        ScalarType GetLengthSquared() const;
        ScalarType GetLength() const;
        bool IsZero() const;
        Vector3<ScalarType> GetNormalized() const;
        void NormalizeInPlace();
        void Set(ScalarType x, ScalarType y, ScalarType z);
        bool Equals(const Vector3<ScalarType> otherVector, ScalarType tolerance = sFloatEpsilon);
        float Dot(const Vector3<ScalarType> otherVector);
        
        // Turns all components that are denormals into 0. Any components that exceed this threshold
        // (positive or negative) get left alone.
        void FloorToZero();

        // Applies an Euler rotation in degrees to this position, returning a copy.
        [[nodiscard]] Position RotateEulerZYX(EulerAngles anglesDegrees) const;

        // Interprets this vector as a local-space position and applies the specified entity position and rotation to it,
        // returning a copy and leaving this instance unmodified.
        [[nodiscard]] Position LocalToWorldPosition(Position entityPosition, EulerAngles entityRotation) const;
        // Transforms this direction vector from local to world space, using an entity's rotation.
        // TODO: make this respect the engine's axis handedness. It's just a wrapper for ZYX rotation for now,
        // which maps to yaw->pitch->roll in Unreal.
        inline Position LocalToWorldDirection(EulerAngles anglesDegrees) { return RotateEulerZYX(anglesDegrees); }

        [[nodiscard]] static Vector3 Lerp(const Vector3& first, const Vector3& second, float lerpT);

        [[nodiscard]] std::string ToString() const;

        // Equality
        bool operator==(const Vector3<ScalarType>& other) const;

        // Vector *= scalar
        Vector3& operator*=(ScalarType scalar);

        // Vector += vector
        Vector3& operator+=(const Vector3& rhs);

        // Vector * scalar
        template<typename T>
        friend Vector3<T> operator*(const Vector3<T>& vector, T scalar);

        // Vector * vector
        template<typename FirstScalarType, typename SecondScalarType>
        friend Vector3<FirstScalarType> operator*(const Vector3<FirstScalarType>& lhs, const Vector3<SecondScalarType>& rhs);
        
        // Vector / scalar
        template<typename T>
        friend Vector3<T> operator/(const Vector3<T>& vector, T scalar);

        template<typename T>
        friend Vector3<T> operator+(const Vector3<T>& lhs, const Vector3<T>& rhs);

        template<typename T>
        friend Vector3<T> operator-(const Vector3<T>& lhs, const Vector3<T>& rhs);
    };
}

#include "Vector3.inl"