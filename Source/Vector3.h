// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <string>
#include "UnitTypes.h"

namespace Kurveball
{
    template<typename ScalarT>
    class Vector3;

    using Float3 = Vector3<float>;
    using Velocity = Float3;
    using Position = Float3;
    using EulerAngles = Float3;

    using Double3 = Vector3<double>;

    // Duck-typed constructor that can initialize from any type that has numeric members called X, Y, and Z.
    template<typename ExternalVectorT>
    Float3 ConvertFloat3(const ExternalVectorT& otherVector);

    // Vector * scalar
    template<typename ScalarT>
    Vector3<ScalarT> operator*(const Vector3<ScalarT>& vector, ScalarT scalar);

    // Vector * vector
    template<typename FirstScalarT, typename SecondScalarT>
    Vector3<FirstScalarT> operator*(const Vector3<FirstScalarT>& lhs, const Vector3<SecondScalarT>& rhs);

    // Vector / scalar
    template<typename ScalarT>
    Vector3<ScalarT> operator/(const Vector3<ScalarT>& vector, ScalarT scalar);

    template<typename ScalarT>
    Vector3<ScalarT> operator+(const Vector3<ScalarT>& lhs, const Vector3<ScalarT>& rhs);

    template<typename ScalarT>
    Vector3<ScalarT> operator-(const Vector3<ScalarT>& lhs, const Vector3<ScalarT>& rhs);
    
    template<typename ScalarT>
    class Vector3
    {
    public:
        using ScalarType = ScalarT;
        Vector3();
        Vector3(ScalarT x, ScalarT y, ScalarT z);

        ScalarT X {};
        ScalarT Y {};
        ScalarT Z {};

        ScalarT GetLengthSquared() const;
        ScalarT GetLength() const;
        bool IsZero() const;
        Vector3<ScalarT> GetNormalized() const;
        void NormalizeInPlace();
        void Set(ScalarT x, ScalarT y, ScalarT z);
        bool Equals(const Vector3<ScalarT> otherVector, ScalarT tolerance = sFloatEpsilon);
        bool Equals(ScalarType x, ScalarType y, ScalarType z, ScalarT tolerance = sFloatEpsilon);
        float Dot(const Vector3<ScalarT> otherVector);
        
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
        [[nodiscard]] inline Position LocalToWorldDirection(EulerAngles anglesDegrees) { return RotateEulerZYX(anglesDegrees); }
        
        [[nodiscard]] inline Position WorldToLocalDirection(EulerAngles anglesDegrees) { return RotateEulerZYX(anglesDegrees * -1.f); }

        [[nodiscard]] static Vector3 Lerp(const Vector3& first, const Vector3& second, ScalarT lerpT);

        [[nodiscard]] std::string ToString() const;
        
        void ToBinary(std::ostream& outStream) const;

        [[nodiscard]] static Vector3 FromBinary(std::istream& istream);

        // Equality
        bool operator==(const Vector3<ScalarT>& other) const;

        // Vector *= scalar
        Vector3& operator*=(ScalarT scalar);

        // Vector += vector
        Vector3& operator+=(const Vector3& rhs);

        // Vector * scalar
        template<typename T>
        friend Vector3<T> operator*(const Vector3<T>& vector, T scalar);

        // Vector * vector
        template<typename FirstScalarT, typename SecondScalarT>
        friend Vector3<FirstScalarT> operator*(const Vector3<FirstScalarT>& lhs, const Vector3<SecondScalarT>& rhs);
        
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