// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "MathUtils.h"
#include <cmath>

namespace CurveLib
{
    float RestrictDegreeRange(float degrees)
    {
        // Avoiding a loop for speed
        float restricted = fmodf(degrees, 360.f);
        if (restricted < 0)
        {
            restricted += 360.f;
        }
        return restricted;
    }

}