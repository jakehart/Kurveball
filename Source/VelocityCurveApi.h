// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <optional>
#include "BlendType.h"
#include "CurveInstanceID.h"
#include "CurveSampler.h"
#include "Axis.h"
#include "UnitTypes.h"
#include "Vector3.h"

namespace Kurveball
{
    struct CurveMechanic;
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

    // Used for telling the curve simulation about the entity's starting position, and for performing teleports
    void SetPosition(VelocityCurveContext& ioCurveContext, float x, float y, float z);
    
    void SetRotation(VelocityCurveContext& ioCurveContext, float x, float y, float z);

    // Returns a 32-bit RGBA color that represents the given curveID. The result is deterministic, so the same curveID
    // is always given a consistent color.
    uint32_t CalculateCurveDebugColor(CurveInstanceID curveID);

    bool IsRotationCurve(const VelocityCurveInstance& curveInstance);

    // Use this when you want your velocity curve's x to depend on something other than time. For example, you
    // could read from different parts of the curve based on your own gameplay variables like health or distance away.
    void DefineCustomCurveXFunction(VelocityCurveContext& ioContext, CurveInstanceID curveID, CurveXFunction func);

    // Transfers the speed from one curve onto another. First, it syncs the toCurve's playback to the speed that
    // most closely matches the fromCurve. Then it executes a blend of your choosing. This is a convenience function
    // that has the same effect as calling SyncToX(FindClosestSpeed()) and then Crossfade().
    void TransferCurve(VelocityCurveContext& ioContext, CurveInstanceID fromCurveID, const CurveInstanceID toCurveID, BlendType blendType = BlendType::Linear, Seconds blendDuration = Seconds{ 0.4f });
    
    // This overload is able to start the "to" curve if it's not already running, since you're passing in a full
    // CurveMechanic description of it.
    void TransferCurve(VelocityCurveContext& ioContext, const CurveMechanic& fromMechanic, const CurveMechanic& toMechanic, BlendType blendType, Seconds blendDuration, bool startToCurveIfNotFound = true);
    
    // Searches an entire velocity curve to find the speed that is closest to desired.
    std::pair<float, MetersPerSecond> FindClosestSpeed(VelocityCurveContext& ioContext, CurveInstanceID curveID, MetersPerSecond desiredSpeed, float searchStartX = 0.f, float stepSize = 0.05f);

    void Crossfade(VelocityCurveContext& ioContext, CurveInstanceID from, CurveInstanceID to, BlendType blendType, Seconds duration);
    void Blend(VelocityCurveContext& ioContext, CurveInstanceID instanceID, BlendType blendType, Seconds duration, bool isBlendIn);
    void Blend(VelocityCurveContext& ioContext, CurveInstanceID instanceID, CurveSamplerTY customBlendFunction);
}