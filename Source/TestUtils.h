// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "AxisMode.h"
#include "CoordinateSpace.h"
#include "UnitTypes.h"
#include "VelocityCurveInstance.h"

#include <cstdlib>

namespace CurveLib
{
    // Generates a bland VelocityCurveInstance that only runs the very basics of
    // velocity curve playback, never engaging the fancy features such as looping
    // or axis masking. My unit tests call this factory function and customize the
    // struct it returns to test the desired features.
    CurveLib::VelocityCurveInstance GenerateTestCurveInstance()
    {
        using namespace CurveLib;

        const auto defaultSamplerFunction = []([[maybe_unused]] float curveX) -> float
            {
                return 1.f;
            };

        VelocityCurveInstance testCurve
        {
            .mMechanic =
            {
                .mInstanceId = std::rand() % std::numeric_limits<CurveInstanceId>::max(),
                .mDebugName = "Test Curve",
                .mDirection = {1, 0, 0},
                .mCoordinateSpace = CoordinateSpace::world,
                .mSpeedMultiplier = 123.f,
                .mAxisMode = AxisMode::allMovementAxes,
                .mStartTime = Seconds(0),
                .mStartPosition = {0, 0, 0},
                .mStretchDuration = Seconds(1),
                .mRawAssetDuration = Seconds(1),
                .mPlayCount = CurveLib::PLAY_COUNT_INFINITE,
                .mLoopStartX = 0,
                .mLoopEndX = 0
            },

            .mDistanceAccumulator = {},
            .mSpeedSampler = defaultSamplerFunction,
            .mOutput = {}
        };

        return testCurve;
    }

    void TickCurveContext(CurveLib::VelocityCurveContext& ioContext, CurveLib::Seconds tickDuration, size_t numTicks)
    {
        using namespace CurveLib;

        // Clear previous state to assure a clean slate for testing
        ioContext.mAbsoluteTime = {};

        for (size_t i = 0; i < numTicks; ++i)
        {
            const Seconds absoluteTime = tickDuration * i;
            CurveLib::TickPlayback(ioContext, absoluteTime);
        }
    }

    CurveLib::Seconds RunCurveAndGetEndTime(CurveLib::VelocityCurveContext& ioContext, CurveLib::VelocityCurveInstance& ioCurveInstance, Seconds tickDuration = Seconds(0.1), size_t maxNumTicks = 10000)
    {
        using namespace CurveLib;

        // Start the timer at zero so that successive curves in the same testcase don't depend on each other
        ioContext.mAbsoluteTime = {};

        StartVelocityCurve(ioContext, ioCurveInstance);

        // Safeguard against infinite execution
        for (uint16_t i = 0; i < maxNumTicks; ++i)
        {
            // Use the short tick time so we can verify end time with more precision
            CurveLib::TickPlayback(ioContext, Seconds(i * tickDuration));

            if (!CurveLib::IsCurveRunning(ioContext, ioCurveInstance.mMechanic.mInstanceId))
            {
                break;
            }
        }

        return ioContext.mAbsoluteTime;
    }
}