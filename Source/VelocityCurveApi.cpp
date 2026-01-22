// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "VelocityCurveApi.h"

#include "VelocityCurveContext.h"
#include "VelocityCurveInstance.h"
#include "VelocityCurvePlayback.h"

namespace CurveLib
{
    void SetVerticalAxis(VelocityCurveContext& ioContext, Axis verticalAxis)
    {
        switch (verticalAxis)
        {
        case Axis::X:
            ioContext.mHorizontalAxisMask = Vector3<bool>(false, true, true);
            ioContext.mVerticalAxisMask = Vector3<bool>(true, false, false);
            break;
        case Axis::Y:
            ioContext.mHorizontalAxisMask = Vector3<bool>(true, false, true);
            ioContext.mVerticalAxisMask = Vector3<bool>(false, true, false);
            break;
        case Axis::Z:
            ioContext.mHorizontalAxisMask = Vector3<bool>(true, true, false);
            ioContext.mVerticalAxisMask = Vector3<bool>(false, false, true);
            break;
        }
    }

    void StartVelocityCurve(VelocityCurveContext& ioContext, const VelocityCurveInstance& newCurve)
    {
        const auto existingCurve{ GetCurveInstance(ioContext, newCurve.mMechanic.mInstanceId) };
        if (existingCurve)
        {
            // TODO: Warn about overwriting, perhaps return based on bool param in settings
        }

        // Create a copy
        VelocityCurveInstance sanitizedCurve = newCurve;
        
        // Fix any invalid parameters and apply conventions
        SanitizeCurveInstance(sanitizedCurve, ioContext);

        if (IsRotationCurve(sanitizedCurve))
        {
            ioContext.mRotationCurves[newCurve.mMechanic.mInstanceId] = sanitizedCurve;
        }
        else
        {
            ioContext.mLinearCurves[newCurve.mMechanic.mInstanceId] = sanitizedCurve;
        }
    }

    void UpdateVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceId instanceId, std::optional<MetersPerSecond> speedMultiplier, std::optional<Float3> direction)
    {
        auto* curveInstance{ AccessCurveInstance(ioContext, instanceId) };
        if (!curveInstance)
        {
            // TODO: Warn
            return;
        }

        // "Bake" the previous motion that the curve caused into its mStartPosition. This prevents the updated speed
        // and direction from applying retroactively to existing progress. 
        //curveInstance->mMechanic.mStartPosition = curveInstance->mMechanic.mStartPosition + curveInstance->mMechanic.mDirection * curveInstance->mMechanic.mDistanceAccumulator.GetTotalArea();
        //curveInstance->mMechanic.mDistanceAccumulator.Reset();

        // Speed and direction can be updated independently, or both together in the same call
        if (speedMultiplier.has_value())
        {
            curveInstance->mMechanic.mSpeedMultiplier = *speedMultiplier;
        }

        if (direction.has_value())
        {
            Float3 sanitizedDirection = *direction;
            if (sanitizedDirection.GetLengthSquared() > 1)
            {
                sanitizedDirection.NormalizeInPlace();
            }

            curveInstance->mMechanic.mDirection = sanitizedDirection;
        }
    }

    void StopVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceId instanceId)
    {
        ioContext.mLinearCurves.erase(instanceId);
        ioContext.mRotationCurves.erase(instanceId);
        // TODO: Curve exit modes or event notification of curve stop
    }

    void StopAllVelocityCurves(VelocityCurveContext& ioContext, bool stopTranslationCurves, bool stopRotationCurves)
    {
        if (stopTranslationCurves)
        {
            ioContext.mLinearCurves.clear();
        }

        if (stopRotationCurves)
        {
            ioContext.mRotationCurves.clear();
        }
    }
    
    void SoftStopVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceId instanceId)
    {
        VelocityCurveInstance* curveInstance = AccessCurveInstance(ioContext, instanceId);
        if (!curveInstance)
        {
            // TODO: log
            return;
        }

        if (IsZero(curveInstance->mMechanic.mLoopStartX) &&
            IsZero(curveInstance->mMechanic.mLoopEndX))
        {
            // This curve has no outro to play, so just stop it immediately
            StopVelocityCurve(ioContext, instanceId);
            return;
        }

        // Prevent further looping
        curveInstance->mMechanic.mPlayCount = 1;

		// Let the outro play
		SeekToX(ioContext, instanceId, curveInstance->mMechanic.mLoopEndX);
    }

    void SeekToX(VelocityCurveContext& ioContext, CurveInstanceId instanceId, float curveXCoordinate)
    {
        VelocityCurveInstance* curveInstance = AccessCurveInstance(ioContext, instanceId);
        if (!curveInstance)
        {
            // TODO: Complain
            return;
        }

        // Convert from stretched playtime back to raw curve asset coordinates
        float conversionFactor = 1.f;
        if (curveInstance->mMechanic.mStretchDuration.count() > CurveLib::sFloatEpsilon)
        {
            conversionFactor = curveInstance->mMechanic.mRawAssetDuration / curveInstance->mMechanic.mStretchDuration;
        }

        // "Backdate" the curve so that the playhead is at the desired X
        curveInstance->mMechanic.mStartTime = ioContext.mAbsoluteTime - Seconds(curveXCoordinate * conversionFactor);
    }

    float GetMechanicSpeed(const VelocityCurveContext& context, CurveInstanceId instanceId)
    {
        const auto* curveInstance = GetCurveInstance(context, instanceId);
        if (!curveInstance)
        {
            return 0.f;
        }
        
        return curveInstance->mOutput.mSpeed;
    }

    float GetTotalSpeed(const VelocityCurveContext& context)
    {
        return context.mOutput.mSpeed;
    }

    void ResetContext(VelocityCurveContext& ioContext)
    {
        ioContext = {};
    }


    bool IsCurveRunning(const VelocityCurveContext& context, CurveInstanceId instanceId)
    {
        return context.mLinearCurves.contains(instanceId) || context.mRotationCurves.contains(instanceId);
    }

    bool IsAnyCurveRunning(const VelocityCurveContext& ioContext, bool includeLinear, bool includeRotational)
    {
        if (includeLinear && !ioContext.mLinearCurves.empty())
        {
            return true;
        }

        if (includeRotational && !ioContext.mRotationCurves.empty())
        {
            return true;
        }

        return false;
    }

    const VelocityCurveInstance* GetCurveInstance(const VelocityCurveContext& context, CurveInstanceId instanceId)
    {
        const auto linearIter{ context.mLinearCurves.find(instanceId) };
        if (linearIter != context.mLinearCurves.end())
        {
            return &linearIter->second;
        }

        const auto rotationIter{ context.mRotationCurves.find(instanceId) };
        if (rotationIter != context.mRotationCurves.end())
        {
            return &rotationIter->second;
        }

        return nullptr;
    }

    VelocityCurveInstance* AccessCurveInstance(VelocityCurveContext& ioContext, CurveInstanceId instanceId)
    {
        const auto linearIter{ ioContext.mLinearCurves.find(instanceId) };
        if (linearIter != ioContext.mLinearCurves.end())
        {
            return &linearIter->second;
        }

        const auto rotationIter{ ioContext.mRotationCurves.find(instanceId) };
        if (rotationIter != ioContext.mRotationCurves.end())
        {
            return &rotationIter->second;
        }

        return nullptr;
    }

    void SanitizeCurveInstance(VelocityCurveInstance& ioCurveInstance, const VelocityCurveContext& context)
    {
        // If the user didn't specify a curve ID, give them a random one
        if (ioCurveInstance.mMechanic.mInstanceId == CurveInstanceId{})
        {
            ioCurveInstance.mMechanic.mInstanceId = std::rand() % std::numeric_limits<CurveInstanceId>::max();
        }

        if (ioCurveInstance.mMechanic.mDirection.IsZero())
        {
            ioCurveInstance.mMechanic.mDirection.Set(1, 0, 0);
        }
        else if(ioCurveInstance.mMechanic.mDirection.GetLengthSquared() > 1)
        {
            ioCurveInstance.mMechanic.mDirection.NormalizeInPlace();
        }

        if (ioCurveInstance.mMechanic.mStartTime.count() < sFloatEpsilon)
        {
            ioCurveInstance.mMechanic.mStartTime = context.mAbsoluteTime;
        }

        if (ioCurveInstance.mMechanic.mStartPosition.IsZero())
        {
            // No starting position specified, so use the last known position
            ioCurveInstance.mMechanic.mStartPosition = context.mOutput.mPosition;
        }
        else if (ioCurveInstance.mMechanic.mCoordinateSpace == CoordinateSpace::local)
        {
            // Interpret the start position in localspace and transform to worldspace
            ioCurveInstance.mMechanic.mStartPosition = ioCurveInstance.mMechanic.mStartPosition.LocalToWorldPosition(context.mOutput.mPosition, context.mOutput.mRotation);
        }

        if (std::abs(ioCurveInstance.mMechanic.mLoopStartX) < sFloatEpsilon &&
            std::abs(ioCurveInstance.mMechanic.mLoopEndX) < sFloatEpsilon)
        {
            ioCurveInstance.mMechanic.mLoopStartX = 0;
            ioCurveInstance.mMechanic.mLoopEndX = (float)ioCurveInstance.mMechanic.mRawAssetDuration.count();
        }

        ioCurveInstance.mOutput = {.mPosition = ioCurveInstance.mMechanic.mStartPosition};
        ioCurveInstance.mHistory.Clear();
    }

    void SetPosition(VelocityCurveContext& ioCurveContext, float x, float y, float z)
    {
        if (ioCurveContext.mOutput.mPosition.Equals(x, y, z))
        {
            return;
        }
        
        ioCurveContext.mPreviousPosition = ioCurveContext.mOutput.mPosition;
        ioCurveContext.mOutput.mPosition.Set(x, y, z);
    }

    void SetRotation(VelocityCurveContext& ioCurveContext, float x, float y, float z)
    {
        ioCurveContext.mOutput.mRotation.Set(x, y, z);
    }

    uint32_t CalculateCurveDebugColor(CurveInstanceId curveId)
    {
        uint32_t color = curveId;
        // Ensure 100% opacity
        color |= 0x000000ff;
        return color;
    }

    bool IsRotationCurve(const VelocityCurveInstance& curveInstance)
    {
        return curveInstance.mMechanic.mAxisMode == AxisMode::yaw ||
                curveInstance.mMechanic.mAxisMode == AxisMode::pitch ||
                curveInstance.mMechanic.mAxisMode == AxisMode::roll;
    }
}