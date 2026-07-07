// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "AxisMode.h"
#include "CoordinateSpace.h"
#include "UnitTypes.h"
#include "VelocityCurveInstance.h"

#include <cstdlib>

namespace Kurveball
{
    const Kurveball::Seconds TICK_DURATION(0.1);
    const Kurveball::Seconds TIME_TOLERANCE = TICK_DURATION * 2.f;
    const Kurveball::Seconds TICK_DURATION_SHORT(0.05);
    const Kurveball::Seconds TIME_TOLERANCE_SHORT = TICK_DURATION_SHORT * 2.f;

    const float DEGREE_TOLERANCE = 1.f;
    const float DISTANCE_TOLERANCE = 0.5f;
    const float SPEED_TOLERANCE = 0.1f;

    struct VelocityCurveContext;

    // Generates a bland VelocityCurveInstance that only runs the very basics of
    // velocity curve playback, never engaging the fancy features such as looping
    // or axis masking. My unit tests call this factory function and customize the
    // struct it returns to test the desired features.
    Kurveball::VelocityCurveInstance GenerateTestCurveInstance();

    void TickCurveContext(Kurveball::VelocityCurveContext& ioContext, Kurveball::Seconds tickDuration, size_t numTicks);

    Kurveball::Seconds RunCurveAndGetEndTime(Kurveball::VelocityCurveContext& ioContext,
                                             Kurveball::VelocityCurveInstance& ioCurveInstance,
                                             Kurveball::Seconds tickDuration = Seconds(0.1),
                                             size_t maxNumTicks = 10000);
}