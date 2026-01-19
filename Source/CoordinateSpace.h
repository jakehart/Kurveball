#pragma once    

namespace CurveLib
{
    enum class CoordinateSpace : std::uint8_t
    {
        local = 0,
        world,

        numEntries
    };
}