#include "UnrealUtils.h"
#include "CurveLib/Vector3.h"

namespace CurveLib
{
	Float3 ToFloat3(const FVector& unrealVector)
	{
		return CurveLib::Float3(unrealVector.X, unrealVector.Y, unrealVector.Z);
	}

	FVector ToFVector(const Float3& curveLibVector)
	{
		return FVector(curveLibVector.X, curveLibVector.Y, curveLibVector.Z);
	}
}