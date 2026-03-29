// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "VelocityCurveApi.h"

#include "Asserts.h"
#include "VelocityCurveContext.h"
#include "VelocityCurveInstance.h"
#include "VelocityCurvePlayback.h"

namespace Kurveball
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
        const auto existingCurve{ GetCurveInstance(ioContext, newCurve.mMechanic.mInstanceID) };
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
            ioContext.mRotationCurves[newCurve.mMechanic.mInstanceID] = sanitizedCurve;
        }
        else
        {
            ioContext.mLinearCurves[newCurve.mMechanic.mInstanceID] = sanitizedCurve;
        }
    }

    void UpdateVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceID instanceID, std::optional<MetersPerSecond> speedMultiplier, std::optional<Float3> direction)
    {
        auto* curveInstance{ AccessCurveInstance(ioContext, instanceID) };
        KURVEBALL_ERROR_RETURN(curveInstance != nullptr, ioContext, ErrorCode::CurveNotFound);

        // "Bake" the previous motion that the curve caused into its mStartPosition. This prevents the updated speed
        // and direction from applying retroactively to existing progress. 
        //curveInstance->mStartPosition = curveInstance->mStartPosition + curveInstance->mMechanic.mDirection * curveInstance->mMechanic.mDistanceAccumulator.GetTotalArea();
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

    void StopVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceID instanceID)
    {
        ioContext.mLinearCurves.erase(instanceID);
        ioContext.mRotationCurves.erase(instanceID);
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
    
    void SoftStopVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceID instanceID)
    {
        VelocityCurveInstance* curveInstance = AccessCurveInstance(ioContext, instanceID);
        if (!curveInstance)
        {
            // TODO: log
            return;
        }

        if (IsZero(curveInstance->mMechanic.mLoopEndX))
        {
            // This curve has no outro to play, so just stop it immediately
            StopVelocityCurve(ioContext, instanceID);
            return;
        }

        // If the playhead is currently before the outro...
        if (CalculateCurveX(ioContext, instanceID) < curveInstance->mMechanic.mLoopEndX)
        {
            // Prevent further looping
            curveInstance->mMechanic.mPlayCount = 1;
        
            // Seek to the outro and let it play
            SeekToX(ioContext, instanceID, curveInstance->mMechanic.mLoopEndX + sFloatEpsilon);
        }
        //else
        {
            // Already in the outro and ending soon, so don't seek backwards
        }
    }

    void SeekToX(VelocityCurveContext& ioContext, CurveInstanceID instanceID, float curveXCoordinate)
    {
        VelocityCurveInstance* curveInstance = AccessCurveInstance(ioContext, instanceID);
        KURVEBALL_ERROR_RETURN(curveInstance != nullptr, ioContext, ErrorCode::CurveNotFound);

        // Zero or negative asset durations are invalid
        KURVEBALL_ERROR_RETURN(curveInstance->mMechanic.mRawAssetDuration.count() > sFloatEpsilon, ioContext, ErrorCode::InvalidCurveAsset);

        // Convert from stretched playtime back to raw curve asset coordinates
        float timeConversionFactor = 1.f;
        if (curveInstance->mMechanic.mStretchDuration.count() > Kurveball::sFloatEpsilon)
        {
            timeConversionFactor = curveInstance->mMechanic.mStretchDuration.count() / curveInstance->mMechanic.mRawAssetDuration.count();
        }

        // "Backdate" the curve so that the playhead is at the desired X
        curveInstance->mMechanic.mStartTime = ioContext.mAbsoluteTime - Seconds(curveXCoordinate * timeConversionFactor);
    }

    Float3 GetMechanicDirection(const VelocityCurveContext& context, CurveInstanceID instanceID)
    {
        const auto* curveInstance = GetCurveInstance(context, instanceID);
        if (!curveInstance)
        {
            return {};
        }

        return curveInstance->mMechanic.mDirection;
    }

    float GetMechanicSpeed(const VelocityCurveContext& context, CurveInstanceID instanceID)
    {
        const auto* curveInstance = GetCurveInstance(context, instanceID);
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


    bool IsCurveRunning(const VelocityCurveContext& context, CurveInstanceID instanceID)
    {
        return context.mLinearCurves.contains(instanceID) || context.mRotationCurves.contains(instanceID);
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

    const VelocityCurveInstance* GetCurveInstance(const VelocityCurveContext& context, CurveInstanceID instanceID)
    {
        const auto linearIter{ context.mLinearCurves.find(instanceID) };
        if (linearIter != context.mLinearCurves.end())
        {
            return &linearIter->second;
        }

        const auto rotationIter{ context.mRotationCurves.find(instanceID) };
        if (rotationIter != context.mRotationCurves.end())
        {
            return &rotationIter->second;
        }

        return nullptr;
    }

    VelocityCurveInstance* AccessCurveInstance(VelocityCurveContext& ioContext, CurveInstanceID instanceID)
    {
        const auto linearIter{ ioContext.mLinearCurves.find(instanceID) };
        if (linearIter != ioContext.mLinearCurves.end())
        {
            return &linearIter->second;
        }

        const auto rotationIter{ ioContext.mRotationCurves.find(instanceID) };
        if (rotationIter != ioContext.mRotationCurves.end())
        {
            return &rotationIter->second;
        }

        return nullptr;
    }

    void SanitizeCurveInstance(VelocityCurveInstance& ioCurveInstance, const VelocityCurveContext& context)
    {
        // If the user didn't specify a curve ID, give them a random one
        if (ioCurveInstance.mMechanic.mInstanceID == CurveInstanceID{})
        {
            ioCurveInstance.mMechanic.mInstanceID = std::rand() % std::numeric_limits<CurveInstanceID>::max();
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

        if (ioCurveInstance.mStartPosition.IsZero())
        {
            // No starting position specified, so use the last known position
            ioCurveInstance.mStartPosition = context.mOutput.mPosition;
        }
        else if (ioCurveInstance.mMechanic.mCoordinateSpace == CoordinateSpace::local)
        {
            // Interpret the start position in localspace and transform to worldspace
            ioCurveInstance.mStartPosition = ioCurveInstance.mStartPosition.LocalToWorldPosition(context.mOutput.mPosition, context.mOutput.mRotation);
        }

        if (std::abs(ioCurveInstance.mMechanic.mLoopStartX) < sFloatEpsilon &&
            std::abs(ioCurveInstance.mMechanic.mLoopEndX) < sFloatEpsilon)
        {
            ioCurveInstance.mMechanic.mLoopStartX = 0;
            ioCurveInstance.mMechanic.mLoopEndX = (float)ioCurveInstance.mMechanic.mRawAssetDuration.count();
        }

        ioCurveInstance.mOutput = {.mPosition = ioCurveInstance.mStartPosition};
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

    uint32_t CalculateCurveDebugColor(CurveInstanceID curveID)
    {
        uint32_t color = curveID;
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

    void DefineCurveXFunction(VelocityCurveContext& ioContext, CurveInstanceID curveID, CurveXFunction func)
    {
        VelocityCurveInstance* curveInstance = AccessCurveInstance(ioContext, curveID);
        KURVEBALL_ERROR_RETURN(curveInstance != nullptr, ioContext, ErrorCode::CurveNotFound);

        curveInstance->mXSampler = func;
    }

    void Crossfade(VelocityCurveContext& ioContext, CurveInstanceID from, CurveInstanceID to, BlendType blendType, Seconds duration)
    {
        // Fade the "from" curve out to zero
        Blend(ioContext, from, blendType, duration, false);
        // Fade the "to" curve in to one (full blast)
        Blend(ioContext, to, blendType, duration, true);
    }

    void Blend(VelocityCurveContext& ioContext, CurveInstanceID instanceID, BlendType blendType, Seconds duration, bool isBlendIn)
    {
        VelocityCurveInstance* curveInstance = AccessCurveInstance(ioContext, instanceID);
        KURVEBALL_ERROR_RETURN(curveInstance != nullptr, ioContext, ErrorCode::CurveNotFound);

        const Seconds blendStartTime = ioContext.mAbsoluteTime;
        const Seconds blendEndTime = ioContext.mAbsoluteTime + duration;
        
        // Direction multiplier: +1.0 for in, -1.0 for out
        const float direction = isBlendIn ? 1.0f : -1.0f;
        const float base = isBlendIn ? 0.0f : 1.0f;

        switch (blendType)
        {
        case BlendType::Cut:
            {
                const float value = isBlendIn ? 1.0f : 0.0f;
                curveInstance->mBlendSampler = [value]([[maybe_unused]] Seconds absoluteTime) { return value; };
            }
        case BlendType::Linear:
        {
            curveInstance->mBlendSampler = [blendStartTime, blendEndTime, direction, base](Seconds now)
            {
                const float progress = (now - blendStartTime).count() / (blendEndTime - blendStartTime).count();
                return std::clamp(base + (direction * progress), 0.f, 1.f);
            };
        }
        default:
            KURVEBALL_ERROR_RETURN(false, ioContext, ErrorCode::BlendTypeNotFound);
        }
    }

    void Blend(VelocityCurveContext& ioContext, CurveInstanceID instanceID, CurveSamplerTY customBlendFunction)
    {
        VelocityCurveInstance* curveInstance = AccessCurveInstance(ioContext, instanceID);
        KURVEBALL_ERROR_RETURN(curveInstance != nullptr, ioContext, ErrorCode::CurveNotFound);

        curveInstance->mBlendSampler = customBlendFunction;
    }
}