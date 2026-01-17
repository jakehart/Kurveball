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

	template<typename ScalarT>
	Vector3<ScalarT>::Vector3()
		:X (0), Y(0), Z(0)
	{
	}

	template<typename ScalarT>
	Vector3<ScalarT>::Vector3(ScalarT x, ScalarT y, ScalarT z)
		: X(x), Y(y), Z(z)
	{
	}

	template<typename ScalarT>
	ScalarT Vector3<ScalarT>::GetLengthSquared() const
	{
		return X * X + Y * Y + Z * Z;
	}

	template<typename ScalarT>
	ScalarT Vector3<ScalarT>::GetLength() const
	{
		return sqrt(GetLengthSquared());
	}

	template<typename ScalarT>
	bool Vector3<ScalarT>::IsZero() const
	{
		return std::abs(X) < sFloatEpsilon &&
				std::abs(Y) < sFloatEpsilon &&
				std::abs(Z) < sFloatEpsilon;
	}


	template<typename ScalarT>
	Vector3<ScalarT> Vector3<ScalarT>::GetNormalized() const
	{
		const ScalarT length{ this->GetLength() };
		if (length > sFloatEpsilon)
		{
			return *this / length;
		}

		return {};
	}

	template<typename ScalarT>
	void Vector3<ScalarT>::NormalizeInPlace()
	{
		const auto length{ this->GetLength() };
		if (length > sFloatEpsilon)
		{
			X /= length;
			Y /= length;
			Z /= length;
		}
	}

	template<typename ScalarT>
	void Vector3<ScalarT>::Set(ScalarT x, ScalarT y, ScalarT z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	template<typename ScalarT>
	bool Vector3<ScalarT>::Equals(const Vector3<ScalarT> otherVector, ScalarT tolerance)
	{
		return std::abs(X - otherVector.X) < tolerance &&
				std::abs(Y - otherVector.Y) < tolerance &&
				std::abs(Z - otherVector.Z) < tolerance;
	}

	template<typename ScalarT>
	float Vector3<ScalarT>::Dot(const Vector3<ScalarT> otherVector)
	{
		return X * otherVector.X + Y * otherVector.Y + Z * otherVector.Z;
	}

	template<typename ScalarT>
	void Vector3<ScalarT>::FloorToZero()
	{
		// From MathUtils.h
		X = CurveLib::FloorToZero(X, sFloatMinDenormal);
		Y = CurveLib::FloorToZero(Y, sFloatMinDenormal);
		Z = CurveLib::FloorToZero(Z, sFloatMinDenormal);
	}

	template<typename ScalarT>
	Position Vector3<ScalarT>::RotateEulerZYX(EulerAngles anglesDegrees) const
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

	template<typename ScalarT>
	Position Vector3<ScalarT>::LocalToWorldPosition(Position entityPosition, EulerAngles entityRotation) const
	{
		const auto rotatedPosition = RotateEulerZYX(entityRotation);
		return rotatedPosition + entityPosition;
	}

	template<typename ScalarT>
	Vector3<ScalarT> Vector3<ScalarT>::Lerp(const Vector3<ScalarT>& first, const Vector3<ScalarT>& second, float lerpT)
	{
		return Lerp3D(first, second, lerpT);
	}

	template<typename ScalarT>
	std::string Vector3<ScalarT>::ToString() const
	{
		std::stringstream ss;
		ss << "(" << X << ", " << Y << ", " << Z << ")";
		return ss.str();
	}

	template<typename ScalarT>
	bool Vector3<ScalarT>::operator==(const Vector3<ScalarT>& other) const
	{
		return std::abs(X - other.X) < sFloatEpsilon &&
				std::abs(Y - other.Y) < sFloatEpsilon &&
				std::abs(Z - other.Z) < sFloatEpsilon;
	}

	template<typename ScalarT>
	Vector3<ScalarT>& Vector3<ScalarT>::operator*=(ScalarT scalar)
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
		return *this;
	}

	template<typename ScalarT>
	Vector3<ScalarT>& Vector3<ScalarT>::operator+=(const Vector3<ScalarT>& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		Z += rhs.Z;
		return *this;
	}

	template<typename ScalarT>
	Vector3<ScalarT> operator*(const Vector3<ScalarT>& vector, ScalarT scalar)
	{
		return Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
	}

	template<typename FirstScalarT, typename SecondScalarT>
	Vector3<FirstScalarT> operator*(const Vector3<FirstScalarT>& lhs, const Vector3<SecondScalarT>& rhs)
	{
		return Vector3(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
	}

	template<typename ScalarT>
	Vector3<ScalarT> operator/(const Vector3<ScalarT>& vector, const ScalarT scalar)
	{
		return Vector3(vector.X / scalar, vector.Y / scalar, vector.Z / scalar);
	}

	template<typename ScalarT>
	Vector3<ScalarT> operator+(const Vector3<ScalarT>& lhs, const Vector3<ScalarT>& rhs)
	{
		return Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
	}

	template<typename ScalarT>
	Vector3<ScalarT> operator-(const Vector3<ScalarT>& lhs, const Vector3<ScalarT>& rhs)
	{
		return Vector3(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
	}

	template<typename ScalarT>
	inline std::ostream& operator<<(std::ostream& os, const Vector3<ScalarT>& vec)
	{
		os << vec.ToString();
		return os;
	}
}