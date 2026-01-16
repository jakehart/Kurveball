// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <array>
#include <vector>

namespace CurveLib
{
	// Implementation of de Casteljau's algorithm to solve a Bezier curve of any degree (linear, quadratic,
	// cubic, etc.) by recursive lerping between point neighbors.
	template<typename CurvePointT, size_t degree>
	class BezierCurveSegment
	{
	public:
		using PointVector = std::vector<CurvePointT>;

		BezierCurveSegment() = default;
		BezierCurveSegment(const PointVector& points);

		bool SetPoints(const PointVector& points);

		// Samples the curve using pure lerp T.
		CurvePointT CalculatePositionAtT(float t) const;
		// Uses the arc length to sample the curve instead of t. This produces a consistent, evenly spaced result, whereas
		// sampling by T causes "expansion" and "contraction"
		CurvePointT CalculatePositionAtDistance(float distance) const;

	private:
		// Lerps between each point and its neighbor according to t, returning the lerped points.
		// The result vector will always be one fewer than the input.
		PointVector CalculateLerpedPoints(const PointVector& inputPoints, float t) const;

		// The control points of this segment. In the case of cubic Bezier (degree 4),
		PointVector mPoints{};

		// Curves with degree 0 or 1 are inherently impossible, since there wouldn't be enough points to lerp between.
		// (They would be impossible in a parametric curve implementation too.)
		// There's no reason I couldn't support degrees over 4, but it's an uncommon use case and would be computationally expensive.
		static_assert(degree > 0 && degree <= 4);
	};
}

#include "BezierCurveSegment.inl"