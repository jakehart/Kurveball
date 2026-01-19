#pragma once

#include "CurveLib/AxisMode.h"

// Wraps CurveLib::AxisMode in a way that Unreal can understand
UENUM(BlueprintType)
enum class EAxisMode : uint8
{
    allMovementAxes = 0 UMETA(DisplayName = "All movement axes"),
    horizontal UMETA(DisplayName = "Horizontal axes only"),
    vertical UMETA(DisplayName = "Vertical axis only"),
    yaw UMETA(DisplayName = "Yaw"),
    pitch UMETA(DisplayName = "Pitch"),
    roll UMETA(DisplayName = "Roll"),
    
    numEntries UMETA(Hidden)
};
static_assert(static_cast<uint8_t>(EAxisMode::numEntries) == static_cast<uint8_t>(CurveLib::AxisMode::numEntries));