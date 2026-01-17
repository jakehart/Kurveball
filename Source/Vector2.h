// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "MathUtils.h"

namespace CurveLib
{
	template<typename ScalarT>
	class Vector2
	{
	public:
		using ScalarType = ScalarT;

		ScalarT X {};
		ScalarT Y {};

		[[nodiscard]] static Vector2 Lerp(const Vector2& first, const Vector2& second, float lerpT)
		{
			return Lerp2D(first, second, lerpT);
		}
	};

	using Float2 = Vector2<float>;
}