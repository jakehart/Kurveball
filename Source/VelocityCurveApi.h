// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <optional>
#include "CurveInstanceID.h"
#include "CurveSampler.h"
#include "Axis.h"
#include "UnitTypes.h"
#include "Vector3.h"

namespace Kurveball
{
    struct VelocityCurveContext;
    struct VelocityCurveInstance;

    // Determines which axis is treated as vertical. The other two will be considered horizontal.
    // You should call this at startup to tell Kurveball which axis your engine considers to be vertical.
    // The default is Y.
    void SetVerticalAxis(VelocityCurveContext& ioContext, Axis verticalAxis);

    void StartVelocityCurve(VelocityCurveContext& ioContext, const VelocityCurveInstance& newCurve);
    void UpdateVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceID instanceID, std::optional<MetersPerSecond> speedMultiplier, std::optional<Float3> direction);
    void StopVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceID instanceID);
    void StopAllVelocityCurves(VelocityCurveContext& ioContext, bool stopTranslationCurves = true, bool stopRotationCurves = true);
    
    // Stops looping the curve, seeks to its mLoopEnd, plays the outro of the curve, and
    // allows it to end naturally.
    void SoftStopVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceID instanceID);

    // Immediately sends the playhead of the velocity curve to a certain x coordinate. This x is relative to the curve
    // asset itself, before any stretching or looping is applied.
    void SeekToX(VelocityCurveContext& ioContext, CurveInstanceID instanceID, float curveXCoordinate);

    Float3 GetMechanicDirection(const VelocityCurveContext& context, CurveInstanceID instanceID);
    float GetMechanicSpeed(const VelocityCurveContext& context, CurveInstanceID instanceID);
    float GetTotalSpeed(const VelocityCurveContext& context);

    // This clears everything back to the startup state. All curve instances are forgotten,
    // absoluteTime is forgotten, EVERYthing.
    void ResetContext(VelocityCurveContext& ioContext);

    bool IsCurveRunning(const VelocityCurveContext& context, CurveInstanceID instanceID);
    bool IsAnyCurveRunning(const VelocityCurveContext& ioContext, bool includeLinear = true, bool includeRotational = true);

    const VelocityCurveInstance* GetCurveInstance(const VelocityCurveContext& context, CurveInstanceID instanceID);
    VelocityCurveInstance* AccessCurveInstance(VelocityCurveContext& ioContext, CurveInstanceID instanceID);
    void SanitizeCurveInstance(VelocityCurveInstance& ioCurveInstance, const VelocityCurveContext& context);

    // Usually, the entity we're controlling will have a starting position, and this function
    // is handy for telling the curve context where that is. Users can also perform teleports this way.
    void SetPosition(VelocityCurveContext& ioCurveContext, float x, float y, float z);
    
    void SetRotation(VelocityCurveContext& ioCurveContext, float x, float y, float z);

    // Returns a 32-bit RGBA color that represents the given curveID. The result is deterministic, so the same curveID
    // is always given a consistent color.
    uint32_t CalculateCurveDebugColor(CurveInstanceID curveID);

    bool IsRotationCurve(const VelocityCurveInstance& curveInstance);

    void DefineCurveXFunction(VelocityCurveContext& ioContext, CurveInstanceID curveID, CurveXFunction func);
}