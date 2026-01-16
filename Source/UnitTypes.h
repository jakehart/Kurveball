// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <chrono>
#include <cstdint>
#include <limits>

namespace CurveLib
{
	constexpr float sFloatMinDenormal{ std::numeric_limits<float>::denorm_min() };
	constexpr float sFloatEpsilon{ std::numeric_limits<float>::epsilon() };
	constexpr std::uint32_t sUint32Max{ std::numeric_limits<std::uint32_t>::max() };
	constexpr float sPi{ 3.141592653589793238462f };
	constexpr float sDegreesToRadians{ sPi / 180.f };

	using MetersPerSecond = float;
	using Seconds = std::chrono::duration<float>;
}