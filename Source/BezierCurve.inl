// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "BezierCurve.h"
#include "Asserts.h"

namespace CurveLib
{
	template<typename CurvePointT>
	CurvePointT BezierCurve<CurvePointT>::CalculatePositionAtT(float t)
	{
		// Binary search to find the Bezier segment that includes the desired t.
		const auto segmentIter = std::lower_bound(mSegments.begin(), mSegments.end(), t, [](const CurveSegment& curveSegment, float searchT)
			{
				return curveSegment.GetStartX() < searchT;
			});

		if (segmentIter == mSegments.end())
		{
			// t is before the beginning of the curve
			return 0.f;
		}

		return segmentIter->CalculatePositionAtT(t);
	}

	template<typename CurvePointT>
	CurveSamplerXY CurveLib::BezierCurve<CurvePointT>::CreateCurveSamplerXY() const
	{
		return [this](float) -> float
			{
				// TODO: need to be able to sample by x
				return -999;
			};
	}

	template<typename CurvePointT>
	CurveSampler3D CurveLib::BezierCurve<CurvePointT>::CreateCurveSampler3D() const
	{
		return [this](float) -> float
			{
				// TODO: need to be able to sample by arc distance
				return {};
			};
	}
}