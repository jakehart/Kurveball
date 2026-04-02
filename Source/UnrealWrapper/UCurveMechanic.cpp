// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md

#if defined(__UNREAL__)

#include "UnrealWrapper/UCurveMechanic.h"

UCurveMechanic::UCurveMechanic()
    :   VelocityCurveAsset(nullptr),
        UniqueName("CurveMechanic"),
        Direction(1.f, 0.f, 0.f),
        CoordinateSpace(ECoordinateSpace::local),
        SpeedMultiplier(1000.f),
        AxisMode(EAxisMode::allMovementAxes),
        StretchDuration(0.f),
        PlayCount(0),
        LoopStartX(0.f),
        LoopEndX(0.f)
{
}

#endif // #if defined(__UNREAL__)