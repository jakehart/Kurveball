// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "UnitTypes.h"

namespace CurveLib
{
	// Restricts a degree measure to [0, 360).
	float RestrictDegreeRange(float degrees);

	template<typename ScalarT>
	ScalarT Lerp(ScalarT first, ScalarT second, ScalarT lerpT)
	{
		return first * (1 - lerpT) + second * lerpT;
	}

	template<typename VectorT, typename ScalarT>
	VectorT Lerp2D(const VectorT& first, const VectorT& second, ScalarT lerpT)
	{
		return VectorT(Lerp(first.X, second.X, lerpT),
						Lerp(first.Y, second.Y, lerpT));
	}

	template<typename VectorT, typename ScalarT>
	VectorT Lerp3D(const VectorT& first, const VectorT& second, ScalarT lerpT)
	{
		return VectorT(Lerp(first.X, second.X, lerpT),
				Lerp(first.Y, second.Y, lerpT),
				Lerp(first.Z, second.Z, lerpT));
	}

	template<typename ScalarT>
	bool IsZero(ScalarT scalar, ScalarT tolerance = sFloatMinDenormal)
	{
		return std::abs(scalar) < tolerance;
	}


	template<typename ScalarT>
	ScalarT FloorToZero(ScalarT scalar, ScalarT tolerance = sFloatMinDenormal)
	{
		if (std::abs(scalar) < tolerance)
		{
			return 0;
		}

		return scalar;
	}
}