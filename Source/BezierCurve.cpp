// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "BezierCurve.h"
#include "Asserts.h"

namespace CurveLib
{
	template<typename CurvePointT, size_t degree>
	CurvePointT BezierCurve<CurvePointT, degree>::CalculatePositionAtT(float t)
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
}