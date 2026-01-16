// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <ranges>
#include <array>
#include <utility>

namespace CurveLib
{
	// Zero-copy join for two arbitrary STL containers
	template<typename ContainerT>
    auto GetJoinedContainerReferences(const ContainerT& first, const ContainerT& second)
    {
        // The type we need to store in the array is a non-owning VIEW of the container.
        using ContainerViewType = std::ranges::ref_view<const ContainerT>;

        // We create a stack-allocated array that holds two ref_view objects.
        // THIS IS SAFE because the caller receives and owns this array.
        return std::array<ContainerViewType, 2>{ first, second };
    }
} // namespace CurveLib