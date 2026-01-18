// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "MathUtils.h"
#include <cmath>

namespace CurveLib
{
	float RestrictDegreeRange(float degrees)
	{
		// Avoiding a loop for speed
		return fmodf(degrees + 1080.f, 360.f);
	}

}