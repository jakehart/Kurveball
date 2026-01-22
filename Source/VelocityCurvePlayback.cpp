// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "VelocityCurvePlayback.h"

#include "Asserts.h"
#include "AxisMode.h"
#include "CoordinateSpace.h"
#include "MathUtils.h"
#include "UnitTypes.h"
#include "VelocityCurveApi.h"
#include "VelocityCurveContext.h"
#include "VelocityCurveInstance.h"
#include "VelocityCurveOutput.h"

namespace CurveLib
{
    void TickPlayback(VelocityCurveContext& ioContext, Seconds absoluteTime)
    {
        // Pause (zero time delta) is fine, but make sure we aren't going back in time inadvertently
        CURVELIB_VERIFY_RETURN(absoluteTime.count() >= ioContext.mAbsoluteTime.count());

        ioContext.mPreviousPosition = ioContext.mOutput.mPosition;

        ioContext.mDeltaTime = absoluteTime - ioContext.mAbsoluteTime;
        ioContext.mAbsoluteTime = absoluteTime;
        
        // Default to a dead stop. If velocity curves are running, they will overwrite these values below.
        // Note that we preserve the position and rotation from the previous tick.
        ioContext.mOutput.mSpeed = 0.f;
        ioContext.mOutput.mVelocity = {};
        ioContext.mOutput.mAngularVelocity.Set(0, 0, 0);
        ioContext.mOutput.mPositionDelta.Set(0, 0, 0);
        ioContext.mOutput.mRotationDelta.Set(0, 0, 0);

        Internal::TickCurvePhase(ioContext, ioContext.mRotationCurves);
        Internal::TickCurvePhase(ioContext, ioContext.mLinearCurves);

        ioContext.mHasEverUpdated = true;
        ioContext.mOutput.mHasEverUpdated = true;
    }

    Seconds CalculateCurveRuntime(const VelocityCurveInstance& curveInstance)
    {
        if (curveInstance.mMechanic.mPlayCount == CurveLib::PLAY_COUNT_INFINITE)
        {
            // Infinite
            return Seconds(0);
        }

        float runtimeStretchFactor = 1.f;
        if (curveInstance.mMechanic.mStretchDuration.count() > sFloatEpsilon)
        {
            // User-specified explicit duration
            runtimeStretchFactor = curveInstance.mMechanic.mStretchDuration / curveInstance.mMechanic.mRawAssetDuration;
        }

        const float rawIntroLength = std::max(0.f, curveInstance.mMechanic.mLoopStartX);
        const float rawOutroLength = std::max(0.f, curveInstance.mMechanic.mRawAssetDuration.count() - curveInstance.mMechanic.mLoopEndX);
        const float rawLoopPortionLength = curveInstance.mMechanic.mLoopEndX - curveInstance.mMechanic.mLoopStartX;
        
        float curveRuntime = rawIntroLength + rawOutroLength + rawLoopPortionLength * (float)curveInstance.mMechanic.mPlayCount;
        curveRuntime *= runtimeStretchFactor;
        return Seconds(curveRuntime);
    }
    
    float CalculateCurveX(const VelocityCurveContext& context, CurveInstanceId curveInstanceId)
    {
        const VelocityCurveInstance* curveInstance = GetCurveInstance(context, curveInstanceId);
        CURVELIB_VERIFY_RETURN(curveInstance, 0.f);

        return Internal::CalculateCurveX(*curveInstance, context.mAbsoluteTime);
    }

    namespace Internal
    {
        float CalculateCurveX(const VelocityCurveInstance& curveInstance, Seconds absoluteTime)
        {
            // Curve x axis represents time in seconds, but it can be stretched and looped.
            // Begin by calculating the time in seconds since the curve started
            float curveX{ static_cast<float>(absoluteTime.count() - curveInstance.mMechanic.mStartTime.count()) };

            // Apply stretching to the entire curve asset before processing loop points
            if (curveInstance.mMechanic.mStretchDuration.count() > sFloatEpsilon)
            {
                const float stretchRatio = curveInstance.mMechanic.mRawAssetDuration.count() / curveInstance.mMechanic.mStretchDuration.count();
                curveX *= stretchRatio;
            }

            // Apply looping. mLoopStartX and mLoopEndX are in terms of the raw x axis of the asset itself.
            if (curveInstance.mMechanic.mPlayCount != 1U && curveX > curveInstance.mMechanic.mLoopEndX)
            {
                const float timePastLoopStart{ curveX - curveInstance.mMechanic.mLoopStartX };
                const float loopPortionLength{ curveInstance.mMechanic.mLoopEndX - curveInstance.mMechanic.mLoopStartX };

                // mod to remove "redundant" playthroughs, leaving only the remainder past the loop start.
                if (loopPortionLength > sFloatEpsilon)
                {
                    curveX = curveInstance.mMechanic.mLoopStartX + fmodf(timePastLoopStart, loopPortionLength);
                }
            }

            return curveX;
        }

        void TickCurvePhase(VelocityCurveContext& ioContext, CurveMap& ioCurvesInPhase)
        {
            if (ioCurvesInPhase.empty())
            {
                return;
            }

            std::vector<CurveInstanceId> curvesToRemove {};
            VelocityCurveOutput& finalOutput = ioContext.mOutput;

            for (auto& [curveId, curveInstance] : ioCurvesInPhase)
            {
                Internal::TickSingleCurve(curveInstance, ioContext);
                Internal::RecordHistory(curveInstance);
                Internal::CombineCurveOutput(finalOutput, curveInstance);

                const Seconds curveRuntime = CalculateCurveRuntime(curveInstance);
                const bool isRuntimeInfinite = curveRuntime.count() < sFloatEpsilon;

                if (!isRuntimeInfinite && ioContext.mAbsoluteTime > curveInstance.mMechanic.mStartTime + curveRuntime)
                {
                    // This curve is over, but avoid deleting while iterating
                    // TODO: Ensure that the perfect, final result is emitted here (to a subtick level)
                    curvesToRemove.push_back(curveId);
                }
            }

            for (const auto curveId : curvesToRemove)
            {
                ioCurvesInPhase.erase(curveId);
            }
        }

        void TickSingleCurve(VelocityCurveInstance& ioCurveInstance, const VelocityCurveContext& context)
        {
            CURVELIB_VERIFY_RETURN(ioCurveInstance.mSpeedSampler);

            const float curvePlayheadX{ CalculateCurveX(ioCurveInstance, context.mAbsoluteTime) };
            const float sampledSpeed{ ioCurveInstance.mSpeedSampler(curvePlayheadX) * ioCurveInstance.mMechanic.mSpeedMultiplier };
            const Float2 curveSample{ context.mAbsoluteTime.count(), sampledSpeed };

            ioCurveInstance.mDistanceAccumulator.AccumulateArea(curveSample);
            
            if (IsRotationCurve(ioCurveInstance))
            {
                ApplyAccumulatorToRotation(ioCurveInstance);
            }
            else
            {
                ApplyAccumulatorToPosition(ioCurveInstance, context);
            }

            // Replace any output denormals with zero for performance
            ioCurveInstance.mOutput.mDirection.FloorToZero();
            ioCurveInstance.mOutput.mPositionDelta.FloorToZero();
            ioCurveInstance.mOutput.mPosition.FloorToZero();
            ioCurveInstance.mOutput.mVelocity.FloorToZero();

            ioCurveInstance.mOutput.mHasEverUpdated = true;
        }

        void ApplyAccumulatorToPosition(VelocityCurveInstance& ioCurveInstance, const VelocityCurveContext& context)
        {
            VelocityCurveOutput& singleOutput = ioCurveInstance.mOutput;
            singleOutput = {};

            if (ioCurveInstance.mMechanic.mCoordinateSpace == CoordinateSpace::local)
            {
                // Interpret the direction in entity-local space and transform it to worldspace.
                // The rest of the output is based on the direction
                singleOutput.mDirection = ioCurveInstance.mMechanic.mDirection.LocalToWorldDirection(context.mOutput.mRotation);
            }
            else
            {
                singleOutput.mDirection = ioCurveInstance.mMechanic.mDirection;
            }

            // The area under the curve is the distance travelled
            const float totalDistance = ioCurveInstance.mDistanceAccumulator.GetTotalArea();

            const float rawSpeedSample = ioCurveInstance.mDistanceAccumulator.GetLatestSample().Y;
            singleOutput.mSpeed = rawSpeedSample;

            singleOutput.mVelocity = singleOutput.mDirection * singleOutput.mSpeed;

            if (ioCurveInstance.mPositionSampler)
            {
                const CurveLib::Float3 splinePosition = (*ioCurveInstance.mPositionSampler)(totalDistance);
                singleOutput.mPositionDelta = splinePosition - singleOutput.mPosition;
                singleOutput.mPosition = splinePosition;
            }
            else
            {
                // Follow a fixed direction from the start position.
                singleOutput.mPosition = ioCurveInstance.mMechanic.mStartPosition + singleOutput.mDirection * totalDistance;
                
                const float distanceThisStep = ioCurveInstance.mDistanceAccumulator.GetLatestStepArea();
                singleOutput.mPositionDelta = singleOutput.mDirection * distanceThisStep;
            }

            MaskCurveOutput(singleOutput, context, ioCurveInstance);
        }

        void ApplyAccumulatorToRotation(VelocityCurveInstance& ioCurveInstance)
        {
            VelocityCurveOutput& singleOutput = ioCurveInstance.mOutput;
            singleOutput = {};

            // The area we accumulated is signed. Portions of the curve that are below zero have negative area,
            // which allows us to rotate both ways
            const float areaUnderCurve = ioCurveInstance.mDistanceAccumulator.GetTotalArea();
            // Normalize to [0, 360). Add a significant offset to get rid of overly negative angles
            // without needing a loop.
            const float accumulatedRotationDegrees = RestrictDegreeRange(areaUnderCurve);

            // The latest integration step contains just the rotation that happened between last tick and this one.
            const float rotationDeltaDegrees = RestrictDegreeRange(ioCurveInstance.mDistanceAccumulator.GetLatestStepArea());

            switch (ioCurveInstance.mMechanic.mAxisMode)
            {
            case AxisMode::yaw:
                // TODO: respect the vertical/right/up axis specified in the context, or output yaw/pitch/roll that the
                // user/glue code interprets. Need to use axis masking for rotation
                singleOutput.mRotation.Z = accumulatedRotationDegrees;
                singleOutput.mRotationDelta.Z = rotationDeltaDegrees;
                break;
            case AxisMode::pitch:
                singleOutput.mRotation.X = accumulatedRotationDegrees;
                singleOutput.mRotationDelta.X = rotationDeltaDegrees;
                break;
            case AxisMode::roll:
                singleOutput.mRotation.Y = accumulatedRotationDegrees;
                singleOutput.mRotationDelta.Y = rotationDeltaDegrees;
                break;
            default:
                // Applying a non-rotation curve as rotation!
                assert(false);
                break;
            }
        }

        void MaskCurveOutput(VelocityCurveOutput& ioCurveOutput, const VelocityCurveContext& context, const VelocityCurveInstance& curveInstance)
        {
            Vector3<bool> axisMask {};

            switch (curveInstance.mMechanic.mAxisMode)
            {
            case AxisMode::allMovementAxes:
                // Pass all axes through, nothing to do
                return;
            case AxisMode::horizontal:
                axisMask = context.mHorizontalAxisMask;
                break;
            case AxisMode::vertical:
                axisMask = context.mVerticalAxisMask;
                break;
            case AxisMode::yaw:
            case AxisMode::pitch:
            case AxisMode::roll:
                // These are rotation modes, so no position should be emitted.
                ioCurveOutput.mPosition = curveInstance.mMechanic.mStartPosition;
                return;
            default:
                // TODO: warn
                return;
            }

            ioCurveOutput.mDirection = (ioCurveOutput.mDirection * axisMask).GetNormalized();
            ioCurveOutput.mVelocity = (ioCurveOutput.mVelocity * axisMask).GetNormalized();
            ioCurveOutput.mSpeed = ioCurveOutput.mVelocity.GetLength();

            ioCurveOutput.mPositionDelta = ioCurveOutput.mPositionDelta * axisMask;
            
            // For the axes that the curve doesn't control, output the position from the previous tick
            Vector3<bool> inverseAxisMask = { !axisMask.X, !axisMask.Y, !axisMask.Z };
            ioCurveOutput.mPosition = (ioCurveOutput.mPosition * axisMask) + (curveInstance.mMechanic.mStartPosition * inverseAxisMask);
        }

        void CombineCurveOutput(VelocityCurveOutput& ioCombinedOutput, const VelocityCurveInstance& instanceToCombine)
        {
            const VelocityCurveOutput& singleOutput = instanceToCombine.mOutput;

            if (!singleOutput.mPositionDelta.IsZero())
            {
                ioCombinedOutput.mVelocity += singleOutput.mVelocity;
                // Total speed from all curves thus far. Can't just sum the speeds because of backtracking
                ioCombinedOutput.mSpeed = ioCombinedOutput.mVelocity.GetLength();

                ioCombinedOutput.mPosition += singleOutput.mPositionDelta;
                ioCombinedOutput.mPositionDelta += singleOutput.mPositionDelta;

                // TODO: Normalize at the very end for proper weighting
                ioCombinedOutput.mDirection += singleOutput.mDirection;
                ioCombinedOutput.mDirection.NormalizeInPlace();
            }
            
            if (!singleOutput.mRotationDelta.IsZero())
            {
                // Add the rotation delta to the combined output rotation, keeping the final result between [0, 360).
                ioCombinedOutput.mRotation.X = RestrictDegreeRange(ioCombinedOutput.mRotation.X + singleOutput.mRotationDelta.X);
                ioCombinedOutput.mRotation.Y = RestrictDegreeRange(ioCombinedOutput.mRotation.Y + singleOutput.mRotationDelta.Y);
                ioCombinedOutput.mRotation.Z = RestrictDegreeRange(ioCombinedOutput.mRotation.Z + singleOutput.mRotationDelta.Z);
            }
        }

        void RecordHistory(VelocityCurveInstance& ioCurveInstance)
        {
            ioCurveInstance.mHistory.AddToEnd(ioCurveInstance.mDistanceAccumulator.GetLatestSample());
        }
    }
}