// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "BezierCurve.h"
#include "Asserts.h"

namespace CurveLib
{
	template<typename CurvePointT>
	CurvePointT BezierCurve<CurvePointT>::CalculatePositionAtT(float t)
	{
		// The whole part determines which segment to sample, and the fractional part determines where
		// within that curve to sample.
		const float megaT = t * mSegments.size();

		// Truncate to get the whole part
		const size_t segmentIndex = static_cast<size_t>(megaT);
		CURVELIB_VERIFY_RETURN(segmentIndex < mSegments.size(), {});

		// Fractional part
		const float tWithinCurve = fmodf(megaT, 1.f);

		return mSegments.at(segmentIndex).CalculatePositionAtT(tWithinCurve);
	}

	template<typename CurvePointT>
	CurveSamplerXY CurveLib::BezierCurve<CurvePointT>::CreateCurveSamplerXY() const
	{
		return [&curve](float) -> float
			{
				// TODO: need to be able to sample by x
				return -999;
			};
	}

	template<typename CurvePointT>
	CurveSampler3D CurveLib::BezierCurve<CurvePointT>::CreateCurveSampler3D() const
	{
		return [&curve](float) -> float
			{
				// TODO: need to be able to sample by arc distance
				return {};
			};
	}
}