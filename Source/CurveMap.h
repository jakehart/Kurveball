// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include "VelocityCurveInstance.h"

#include <map>

namespace Kurveball
{
    using CurveMap = std::unordered_map<CurveInstanceId, VelocityCurveInstance>;
}