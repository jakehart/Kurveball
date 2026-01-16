// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <functional>
#include "Vector3.h"

namespace CurveLib
{
	// A function that looks at a raw x coordinate on a curve and returns the y there. Generally
	// a lambda that captures the curve data itself.
	using CurveSampler3D = std::function<Float3(float)>;
}