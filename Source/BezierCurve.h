// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "BezierCurveSegment.h"
#include "CurveSampler.h"
#include <vector>

namespace CurveLib
{
	template<typename CurvePointT>
	class BezierCurve
	{
	public:
		using CurveSegment = BezierCurveSegment<CurvePointT>;
		CurvePointT CalculatePositionAtT(float t);

		CurveSamplerXY CreateCurveSamplerXY(const BezierCurve& curve);

	private:
		// TODO: Knot points should not use redundant memory
		std::vector<CurveSegment> mSegments{};
	};
}

#include "BezierCurve.inl"