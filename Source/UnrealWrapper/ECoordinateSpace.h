#pragma once

#include "CurveLib/CoordinateSpace.h"

// Wraps CurveLib::CoordinateSpace for Unreal
UENUM(BlueprintType)
enum class ECoordinateSpace : uint8
{
    local = 0,
    world,

    numEntries UMETA(Hidden)
};
static_assert(static_cast<uint8_t>(ECoordinateSpace::numEntries) == static_cast<uint8_t>(CurveLib::CoordinateSpace::numEntries));