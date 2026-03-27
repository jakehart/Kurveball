// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "CurveInstanceId.h"
#include "CircularBuffer.h"
#include "Axis.h"
#include "CurveMap.h"
#include "VelocityCurveOutput.h"

namespace Kurveball
{
    static constexpr size_t sNumRecordedTicks{ 4U };
    static_assert(sNumRecordedTicks > 0U);

    struct VelocityCurveContext
    {
        CurveMap mLinearCurves{};
        CurveMap mRotationCurves{};

        // Absolute time since start, as received from the user and saved by TickPlayback. (Usually excludes pause time)
        Seconds mAbsoluteTime{ 0 };
        
        // Automatically updated in TickPlayback, based on the current vs. previous mAbsoluteTime.
        Seconds mDeltaTime{ 0 };

        // The most recent output from TickPlayback().
        VelocityCurveOutput mOutput;
        bool mHasUpdated{ false };

        Float3 mPreviousPosition{};

        // These are set by Kurveball::SetVerticalAxis, and they denote which axes "survive" when
        // applying AxisMode::horizontal or AxisMode::vertical.
        Vector3<bool> mVerticalAxisMask{ false, true, false };
        Vector3<bool> mHorizontalAxisMask{ true, false, true };

        // TODO: Add optional debug recording using the CurveOutputs
    };
}