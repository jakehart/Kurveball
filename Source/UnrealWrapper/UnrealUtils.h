#pragma once

namespace CurveLib
{
	template<typename ScalarType>
	class Vector3;
	using Float3 = Vector3<float>;

	Float3 ToFloat3(const FVector& unrealVector);
	FVector ToFVector(const Float3& curveLibVector);
}