// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <functional>

#include "Vector2.h"

namespace Kurveball
{
    // Calculus integration function for a single sample of the area under the curve. 
    // Performs Riemann, trapezoidal, or another approximation algorithm. See IntegratorFunctions.h
    using IntegratorFunction = std::function<float(const Float2&, const Float2&)>;
}