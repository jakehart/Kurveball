// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <optional>
#include "CurveInstanceId.h"
#include "CurveSampler.h"
#include "Axis.h"
#include "UnitTypes.h"
#include "Vector3.h"

namespace CurveLib
{
    struct VelocityCurveContext;
    struct VelocityCurveInstance;

    // Determines which axis is treated as vertical. The other two will be considered horizontal.
    // You should call this at startup to tell CurveLib which axis your engine considers to be vertical.
    // The default is Y.
    void SetVerticalAxis(VelocityCurveContext& ioContext, Axis verticalAxis);

    void StartVelocityCurve(VelocityCurveContext& ioContext, const VelocityCurveInstance& newCurve);
    void UpdateVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceId instanceId, std::optional<MetersPerSecond> speedMultiplier, std::optional<Float3> direction);
    void StopVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceId instanceId);
    void StopAllVelocityCurves(VelocityCurveContext& ioContext, bool stopTranslationCurves = true, bool stopRotationCurves = true);
    
    // Stops looping the curve, seeks to its mLoopEnd, plays the outro of the curve, and
    // allows it to end naturally.
    void SoftStopVelocityCurve(VelocityCurveContext& ioContext, CurveInstanceId instanceId);

    void SeekToX(VelocityCurveContext& ioContext, CurveInstanceId instanceId, float curveXCoordinate);

    Float3 GetMechanicDirection(const VelocityCurveContext& context, CurveInstanceId instanceId);
    float GetMechanicSpeed(const VelocityCurveContext& context, CurveInstanceId instanceId);
    float GetTotalSpeed(const VelocityCurveContext& context);

    // This clears everything back to the startup state. All curve instances are forgotten,
    // absoluteTime is forgotten, EVERYthing.
    void ResetContext(VelocityCurveContext& ioContext);

    bool IsCurveRunning(const VelocityCurveContext& context, CurveInstanceId instanceId);
    bool IsAnyCurveRunning(const VelocityCurveContext& ioContext, bool includeLinear = true, bool includeRotational = true);

    const VelocityCurveInstance* GetCurveInstance(const VelocityCurveContext& context, CurveInstanceId instanceId);
    VelocityCurveInstance* AccessCurveInstance(VelocityCurveContext& ioContext, CurveInstanceId instanceId);
    void SanitizeCurveInstance(VelocityCurveInstance& ioCurveInstance, const VelocityCurveContext& context);

    // Usually, the entity we're controlling will have a starting position, and this function
    // is handy for telling the curve context where that is. Users can also perform teleports this way.
    void SetPosition(VelocityCurveContext& ioCurveContext, float x, float y, float z);
    
    void SetRotation(VelocityCurveContext& ioCurveContext, float x, float y, float z);

    // Returns a 32-bit RGBA color that represents the given curveId. The result is deterministic, so the same curveId
    // is always given a consistent color.
    uint32_t CalculateCurveDebugColor(CurveInstanceId curveId);

    bool IsRotationCurve(const VelocityCurveInstance& curveInstance);

    void DefineCurveXFunction(VelocityCurveContext& ioContext, CurveInstanceId curveID, CurveXFunction func);
}