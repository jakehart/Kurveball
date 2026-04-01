#pragma once

#include "BlendType.h"

// Wraps Kurveball::BlendType in a way that Unreal can understand
UENUM(BlueprintType)
enum class EBlendType : uint8
{
    // Blend the curve in or out at a consistent rate
    Linear,
    // The transition happens instantly, with no blend time
    Cut,
    
    numEntries UMETA(Hidden)
};
static_assert(static_cast<uint8_t>(EAxisMode::numEntries) == static_cast<uint8_t>(Kurveball::AxisMode::numEntries));