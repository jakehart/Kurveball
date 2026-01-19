#pragma once    

namespace CurveLib
{
    enum class AxisMode : std::uint8_t
    {
        allMovementAxes = 0, // All linear axes, excluding rotation
        horizontal,
        vertical,
        yaw,
        pitch,
        roll,

        numEntries
    };
}