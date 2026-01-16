#pragma once
#include <cmath>
#include <sstream>
#include "MathUtils.h"
#include "UnitTypes.h"

namespace CurveLib
{
	template<typename ExternalVectorT>
	Float3 ConvertFloat3(const ExternalVectorT& otherVector)
	{
		return Float3(otherVector.X, otherVector.Y, otherVector.Z);
	}

	template<typename ScalarType>
	Vector3<ScalarType>::Vector3()
		:X (0), Y(0), Z(0)
	{
	}

	template<typename ScalarType>
	Vector3<ScalarType>::Vector3(ScalarType x, ScalarType y, ScalarType z)
		: X(x), Y(y), Z(z)
	{
	}

	template<typename ScalarType>
	ScalarType Vector3<ScalarType>::GetLengthSquared() const
	{
		return X * X + Y * Y + Z * Z;
	}

	template<typename ScalarType>
	ScalarType Vector3<ScalarType>::GetLength() const
	{
		return sqrt(GetLengthSquared());
	}

	template<typename ScalarType>
	bool Vector3<ScalarType>::IsZero() const
	{
		return std::abs(X) < sFloatEpsilon &&
				std::abs(Y) < sFloatEpsilon &&
				std::abs(Z) < sFloatEpsilon;
	}


	template<typename ScalarType>
	Vector3<ScalarType> Vector3<ScalarType>::GetNormalized() const
	{
		const ScalarType length{ this->GetLength() };
		if (length > sFloatEpsilon)
		{
			return *this / length;
		}

		return {};
	}

	template<typename ScalarType>
	void Vector3<ScalarType>::NormalizeInPlace()
	{
		const auto length{ this->GetLength() };
		if (length > sFloatEpsilon)
		{
			X /= length;
			Y /= length;
			Z /= length;
		}
	}

	template<typename ScalarType>
	void Vector3<ScalarType>::Set(ScalarType x, ScalarType y, ScalarType z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	template<typename ScalarType>
	bool Vector3<ScalarType>::Equals(const Vector3<ScalarType> otherVector, ScalarType tolerance)
	{
		return std::abs(X - otherVector.X) < tolerance &&
				std::abs(Y - otherVector.Y) < tolerance &&
				std::abs(Z - otherVector.Z) < tolerance;
	}

	template<typename ScalarType>
	float Vector3<ScalarType>::Dot(const Vector3<ScalarType> otherVector)
	{
		return X * otherVector.X + Y * otherVector.Y + Z * otherVector.Z;
	}

	template<typename ScalarType>
	void Vector3<ScalarType>::FloorToZero()
	{
		// From MathUtils.h
		X = CurveLib::FloorToZero(X, sFloatMinDenormal);
		Y = CurveLib::FloorToZero(Y, sFloatMinDenormal);
		Z = CurveLib::FloorToZero(Z, sFloatMinDenormal);
	}

	template<typename ScalarType>
	Position Vector3<ScalarType>::RotateEulerZYX(EulerAngles anglesDegrees) const
	{
		Vector3 result = *this;

		EulerAngles anglesRad = EulerAngles(anglesDegrees.X * sDegreesToRadians,
											anglesDegrees.Y * sDegreesToRadians,
											anglesDegrees.Z * sDegreesToRadians);

		// Pre-calculate sin and cos values
		Float3 sines = Float3(sin(anglesRad.X), sin(anglesRad.Y), sin(anglesRad.Z));
		Float3 cosines = Float3(cos(anglesRad.X), cos(anglesRad.Y), cos(anglesRad.Z));

		// Apply rotation around Z (yaw)
		float tempX = result.X;
		result.X = tempX * cosines.Z - result.Y * sines.Z;
		result.Y = tempX * sines.Z + result.Y * cosines.Z;

		// Y (pitch)
		// RY * (RZ * pos)
		tempX = result.X;
		float tempZ = result.Z;
		result.X = tempX * cosines.Y + tempZ * sines.Y;
		result.Z = tempZ * cosines.Y - tempX * sines.Y;

		// X (roll)
		// RX * (RY * RZ * pos)
		float tempY = result.Y;
		tempZ = result.Z;
		result.Y = tempY * cosines.X - tempZ * sines.X;
		result.Z = tempY * sines.X + tempZ * cosines.X;

		return result;
	}

	template<typename ScalarType>
	Position Vector3<ScalarType>::LocalToWorldPosition(Position entityPosition, EulerAngles entityRotation) const
	{
		const auto rotatedPosition = RotateEulerZYX(entityRotation);
		return rotatedPosition + entityPosition;
	}

	template<typename ScalarType>
	Vector3<ScalarType> Vector3<ScalarType>::Lerp(const Vector3<ScalarType>& first, const Vector3<ScalarType>& second, float lerpT)
	{
		return Lerp3D(first, second, lerpT);
	}

	template<typename ScalarType>
	std::string Vector3<ScalarType>::ToString() const
	{
		std::stringstream ss;
		ss << "(" << X << ", " << Y << ", " << Z << ")";
		return ss.str();
	}

	template<typename ScalarType>
	bool Vector3<ScalarType>::operator==(const Vector3<ScalarType>& other) const
	{
		return std::abs(X - other.X) < sFloatEpsilon &&
				std::abs(Y - other.Y) < sFloatEpsilon &&
				std::abs(Z - other.Z) < sFloatEpsilon;
	}

	template<typename ScalarType>
	Vector3<ScalarType>& Vector3<ScalarType>::operator*=(ScalarType scalar)
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
		return *this;
	}

	template<typename ScalarType>
	Vector3<ScalarType>& Vector3<ScalarType>::operator+=(const Vector3<ScalarType>& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		Z += rhs.Z;
		return *this;
	}

	template<typename ScalarType>
	Vector3<ScalarType> operator*(const Vector3<ScalarType>& vector, ScalarType scalar)
	{
		return Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
	}

	template<typename FirstScalarType, typename SecondScalarType>
	Vector3<FirstScalarType> operator*(const Vector3<FirstScalarType>& lhs, const Vector3<SecondScalarType>& rhs)
	{
		return Vector3(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
	}

	template<typename ScalarType>
	Vector3<ScalarType> operator/(const Vector3<ScalarType>& vector, const ScalarType scalar)
	{
		return Vector3(vector.X / scalar, vector.Y / scalar, vector.Z / scalar);
	}

	template<typename ScalarType>
	Vector3<ScalarType> operator+(const Vector3<ScalarType>& lhs, const Vector3<ScalarType>& rhs)
	{
		return Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
	}

	template<typename ScalarType>
	Vector3<ScalarType> operator-(const Vector3<ScalarType>& lhs, const Vector3<ScalarType>& rhs)
	{
		return Vector3(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
	}

	template<typename ScalarType>
	inline std::ostream& operator<<(std::ostream& os, const Vector3<ScalarType>& vec)
	{
		os << vec.ToString();
		return os;
	}
}