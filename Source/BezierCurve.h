// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "BezierCurveSegment.h"
#include <vector>

namespace CurveLib
{
	template<typename CurvePointT>
	class BezierCurve
	{
	public:
		using CurveSegment = BezierCurveSegment<CurvePointT>;
		CurvePointT CalculatePositionAtT(float t);

	private:
		std::vector<CurveSegment> mSegments{};
	};
}