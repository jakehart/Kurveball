#pragma once

namespace Kurveball
{
    enum class BlendType : uint8_t
    {
        // Blend the curve in or out at a consistent rate
        Linear,
        // The transition happens instantly, with no blend time
        Cut
    };
}