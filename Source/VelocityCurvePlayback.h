// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "CurveMap.h"
#include "UnitTypes.h"

namespace Kurveball
{
    struct VelocityCurveContext;
    struct VelocityCurveInstance;
    struct VelocityCurveOutput;
    enum class AxisMode : std::uint8_t;

    // Ticks the velocity curve context from a variable timestep, e.g. from the Update function in Unity, or TickComponent in Unreal.
    // A variable timestep means that the time between calls is not always evenly spaced.
    void VariableTickPlayback(VelocityCurveContext& ioContext, Seconds absoluteTime);

    // Ticks the velocity curve context when called from a fixed tick (e.g. the physics tick, or FixedUpdate in Unity).
    // If you have a variable tick, or if you don't know for sure, call VariableTickPlayback.
    void FixedTickPlayback(VelocityCurveContext& ioContext, Seconds absoluteTime);

    // Returns the time that a curve will take to run, including its loop and stretch parameters.
    // If the curve will run forever, returns zero.
    Seconds CalculateTotalRuntime(const VelocityCurveInstance& curveInstance);

    // Returns the current position of the playhead on the curve asset, expressed in the raw
    // x coordinate of the curve itself (with no looping or stretching applied).
    float CalculateCurveX(const VelocityCurveContext& context, CurveInstanceID curveInstanceID);
    
    // Helper functions
    namespace Internal
    {
        // Given the current time, returns the x coordinate of the velocity curve's playhead.
        // The curve's x axis is time in seconds.
        float CalculateCurveX(const VelocityCurveInstance& curveInstance, Seconds absoluteTime);

        void TickCurvePhase(VelocityCurveContext& ioContext, CurveMap& ioCurvesInPhase);

        void TickSingleCurve(VelocityCurveInstance& ioCurveInstance, const VelocityCurveContext& context);

        // Applies the curve's mDistanceAccumulator to its mOutput. Depending on mAxisMode,
        // the accumulated distance is applied to either position (producing motion) or
        // to a rotational axis.
        void ApplyAccumulatorToPosition(VelocityCurveInstance& ioCurveInstance, const VelocityCurveContext& context);
        void ApplyAccumulatorToRotation(VelocityCurveInstance& ioCurveInstance);
        
        void MaskCurveOutput(VelocityCurveOutput& ioCurveOutput, const VelocityCurveContext& context, const VelocityCurveInstance& curveInstance);

        // Folds the output from singleCurveOutput into ioCombinedOutput.
        void CombineCurveOutput(VelocityCurveOutput& ioCombinedOutput, const VelocityCurveInstance& instanceToCombine);

        // Records a single curve's current (time, speed) output into its mHistory buffer.
        void RecordHistory(VelocityCurveInstance& ioCurveInstance);
    }
}