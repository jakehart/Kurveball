// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <array>
#include <ostream>
#include <vector>

namespace CurveLib
{
	// Implementation of de Casteljau's algorithm to solve a Bezier curve of any degree (linear, quadratic,
	// cubic, etc.) by recursive lerping between point neighbors.
	template<typename CurvePointT>
	class BezierCurveSegment
	{
	public:
		using PointVector = std::vector<CurvePointT>;
		using ScalarType = CurvePointT::ScalarType;
		uint8_t MAX_SUPPORTED_POINTS = 4; // Cubic Bezier

		BezierCurveSegment() = default;
		BezierCurveSegment(const PointVector& points);

		void SetPoints(const PointVector& points);
		const std::vector<CurvePointT>& GetPoints() const;
		std::vector<CurvePointT>& AccessPoints();

		ScalarType GetStartX() const;

		// Samples the curve by pure T, using De Casteljau's algorithm.
		CurvePointT CalculatePositionAtT(ScalarType t) const;
		// Uses the arc length to sample the curve instead of t. This produces a consistent, evenly spaced result, whereas
		// sampling by T causes "expansion" and "contraction"
		CurvePointT CalculatePositionAtDistance(ScalarType distance) const;

		void ToBinary(std::ostream& outStream) const;
		[[nodiscard]] static BezierCurveSegment FromBinary(std::istream& istream);

	private:
		using PointInfluenceVector = std::vector<ScalarType>;

		// Uses Bernstein basis polynomials to calculate the "influence" coefficients of each point in this segment.
		PointInfluenceVector CalculatePointInfluences(ScalarType t) const;

		// The control points of this segment. For example, in the case of cubic Bezier (polynomial degree 3), there are 4 points contained here.
		// TODO: Use std::view on a list of points on the parent BezierCurve
		PointVector mPoints{};
	};
}

#include "BezierCurveSegment.inl"