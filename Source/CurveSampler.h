// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <functional>
#include "UnitTypes.h"

namespace Kurveball
{
    struct VelocityCurveInstance;

    // A function that looks at a curve's raw x coordinate (not t) and returns the y there. Generally
    // a lambda that captures the curve data itself.
    using CurveSamplerXY = std::function<float(float)>;
    
    // Used for blend functions. Takes an absolute time value and uses it as the t to sample a blend curve.
    using CurveSamplerTY = std::function<float(Seconds)>;

    // Defined by the user to determine the x coordinate (playhead) of the curve for themselves.
    using CurveXFunction = std::function<float()>;
}