#pragma once

#include "AxisMode.h"
#include "CoordinateSpace.h"
#include "CurveInstanceID.h"
#include "UnitTypes.h"
#include "Vector3.h"

#include <optional>
#include <string>

namespace Kurveball
{
    constexpr size_t PLAY_COUNT_INFINITE = 0U;

    struct CurveMechanic
    {
        // The unique ID for this mechanic, as set by the user. This is used to tell Kurveball which
        // curves to start, stop, and update. It is usually calculated from the string hash of mDebugName.
        CurveInstanceID mInstanceID{};
        
        std::optional<std::string> mDebugName{};

        // The direction you want to go, specified in terms of mCoordinateSpace.
        Float3 mDirection{};
        
        // Choose whether you want the velocity curve to run in the entity's localspace
        // or in worldspace.
        CoordinateSpace mCoordinateSpace{ CoordinateSpace::world };
        
        // The scale factor for your mechanic.This is automatically multiplied with the vertical axis of your Velocity Curve Asset to generate the final speed.
        MetersPerSecond mSpeedMultiplier{ 0.f };

        // Masks the mechanic's output so that it only affects the axes you want, leaving the others alone.
        AxisMode mAxisMode{ AxisMode::allMovementAxes };

        // If zero, this is populated to the current time when the curve is started. Otherwise,
        // this is in terms of absoluteTime.
        // TODO: Express this in terms of delay for better devX?
        Seconds mStartTime{ 0.f };

        // By convention, a stretch duration of zero means to play the velocity curve with no stretching,
        // at its authored duration (mRawAssetDuration)
        Seconds mStretchDuration{ 0.f };

        // This is the x-width of the actual curve asset, starting at zero. This is the duration that
        // the velocity curve would have without timestretching.
        Seconds mRawAssetDuration {};

        // By convention, a play count of zero means "loop forever." Any other count is interpreted literally,
        // e.g. 1 to play the curve once total, 3 to play it 3 times, etc.
        uint32_t mPlayCount{ 1U };
        
        // Loop points allow you to customize which part of the curve gets looped. You can
        // create a curve with an intro that plays once, a looped midsection that plays
        // some number of times according to PlayCount, and an outro that plays once.
        // 
        // Loop start and end are in terms of the x axis of the velocity curve itself (not
        // including any timestretching or looping that might be applied).
        // By convention, mLoopEndX = 0 means that the entire curve should loop.
        float mLoopStartX{ 0.f };
        float mLoopEndX{ 0.f };
    };
}