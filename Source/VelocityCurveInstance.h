// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <cstdint>
#include <chrono>

#include "AreaAccumulator.h"
#include "CircularBuffer.h"
#include "CurveSampler.h"
#include "CurveSampler3D.h"
#include "CurveMechanic.h"
#include "VelocityCurveOutput.h"

namespace Kurveball
{
    constexpr size_t NUM_HISTORY_ENTRIES = 256U;

    struct VelocityCurveInstance
    {
        // The curve's description as received from the user and sanitized in StartVelocityCurve.
        CurveMechanic mMechanic {};
        
        // Each tick, TickPlayback (in VelocityCurvePlayback.h) samples the speed from the velocity curve,
        // then feeds it into this accumulator that calculates the area under the curve at each update step
        // (which is used to calculate position delta), and adds it to the total area (which corresponds to
        // total distance travelled and is used to calculate the current position).
        AreaAccumulator mDistanceAccumulator {};
        
        // Given an x coordinate, returns a speed value (y). This is the function used to sample the velocity curve asset.
        CurveSamplerXY mSpeedSampler;
        
        // Given some distance travelled, returns a position. This exists so that we can use a velocity curve
        // to move along a 3D world-space spline.
        std::optional<CurveSampler3D> mPositionSampler = std::nullopt;

        // By default, velocity curves play out over time, looping and stretching as defined by the parameters in CurveMechanic.
        // But in advanced use cases, you might want to use some other means to decide the x coordinate that is sampled from the
        // curve. For example, perhaps you want the X to depend upon distance above a certain plane, or upon speed, or even upon user
        // input. If you define this optional function, you can bind whatever data you need into your lambda and return a float
        // that determines the x coordinate to sample.
        std::optional<CurveXFunction> mXSampler = std::nullopt;
        
        // The output (position, rotation, speed, etc.) of this individual curve from the most recent tick.
        // This is combined with any other running curves to give the final result.
        Kurveball::VelocityCurveOutput mOutput {};
        
        // Saves a history of this curve's most recent output velocities (with speedMultiplier multiplied in)
        // for debug purposes.
        // TODO: Make debug information completely optional to save memory, and make its size adjustable by the user
        CircularBuffer<Float2, NUM_HISTORY_ENTRIES> mHistory {};

        std::string ToString() const
        {
            std::stringstream ss;

            const std::string debugName = mMechanic.mDebugName ? *mMechanic.mDebugName : "Unnamed";
            ss << debugName << ": "
                << mMechanic.mSpeedMultiplier << " m/s, dir "
                << mMechanic.mDirection.ToString()
                << ", stretchLen: " << mMechanic.mStretchDuration;

            return ss.str();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const VelocityCurveInstance& instance)
    {
        os << instance.ToString();
        return os;
    }
}