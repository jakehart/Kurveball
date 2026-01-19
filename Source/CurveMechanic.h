#pragma once

#include "AxisMode.h"
#include "CoordinateSpace.h"
#include "CurveInstanceId.h"
#include "UnitTypes.h"
#include "Vector3.h"

#include <optional>
#include <string>

namespace CurveLib
{
    constexpr size_t PLAY_COUNT_INFINITE = 0U;

    struct CurveMechanic
    {
        // TODO: Autopopulate this from the std::hash of the debug name if available and the instance ID is unset
        CurveInstanceId mInstanceId{};
        
        std::optional<std::string> mDebugName{};

        // The direction you want to go, specified in terms of mCoordinateSpace.
        Float3 mDirection{};
        
        // Choose whether you want the velocity curve to run in the entity's localspace
        // or in worldspace.
        CoordinateSpace mCoordinateSpace{ CoordinateSpace::world };
        
        MetersPerSecond mSpeedMultiplier{ 0.f };
        AxisMode mAxisMode{ AxisMode::allMovementAxes };

        // If zero, this is populated to the current time when the curve is started. Otherwise,
        // this is in terms of absoluteTime.
        // TODO: Express this in terms of delay for better devX?
        Seconds mStartTime{ 0.f };
        // If zero, uses the last known position.
        Position mStartPosition{};

        // By convention, a stretch duration of zero means to play the velocity curve with no stretching,
        // at its authored duration (mRawAssetDuration)
        Seconds mStretchDuration{ 0.f };

        // This is just the x-width of the actual curve asset, starting at zero. This is the duration that
        // the velocity curve would have without timestretching.
        Seconds mRawAssetDuration {};

        // By convention, a play count of zero means "loop forever." Any other count is interpreted literally,
        // e.g. 1 to play the curve once total, 3 to play it 3 times, etc.
        uint32_t mPlayCount{ 1U };
        
        // Loop start and end are in terms of the x axis of the velocity curve itself (not
        // including any timestretching or looping that might be applied).
        float mLoopStartX{ 0.f };
        // By convention, mLoopEnd = 0 means that the loop includes the entire authored curve.
        float mLoopEndX{ 0.f };
    };
}