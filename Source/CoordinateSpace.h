#pragma once    

namespace Kurveball
{
    enum class CoordinateSpace : std::uint8_t
    {
        local = 0,
        world,

        numEntries
    };
}