// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "BezierCurveSegment.h"
#include "CurveSampler.h"
#include "CurveSampler3D.h"
#include <vector>

namespace CurveLib
{
	template<typename CurvePointT>
	class BezierCurve
	{
	public:
		using CurveSegment = BezierCurveSegment<CurvePointT>;
		CurvePointT CalculatePositionAtT(float t);

		[[nodiscard]] CurveSamplerXY CreateCurveSamplerXY() const;
		[[nodiscard]] CurveSampler3D CreateCurveSampler3D() const;

	private:
		// TODO: Knot points should not use redundant memory. Can use std::view for this
		std::vector<CurveSegment> mSegments{};
	};
}

#include "BezierCurve.inl"