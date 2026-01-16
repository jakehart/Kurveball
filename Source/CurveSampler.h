// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <functional>

namespace CurveLib
{
	// A function that looks at a raw x coordinate on a curve and returns the y there. Generally
	// a lambda that captures the curve data itself.
	using CurveSampler = std::function<float(float)>;
}