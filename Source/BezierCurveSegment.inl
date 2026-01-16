// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "MathUtils.h"
#include "Asserts.h"

namespace CurveLib
{
	template<typename CurvePointT, size_t degree>
	BezierCurveSegment<CurvePointT, degree>::BezierCurveSegment(const BezierCurveSegment::PointVector& points)
	{
		// TODO: Complain if SetPoints fails
		SetPoints(points);
	}

	template<typename CurvePointT, size_t degree>
	bool BezierCurveSegment<CurvePointT, degree>::SetPoints(const PointVector& points)
	{
		if (points.size() != degree)
		{
			return false;
		}

		mPoints = points;
		return true;
	}

	template<typename CurvePointT, size_t degree>
	CurvePointT BezierCurveSegment<CurvePointT, degree>::CalculatePositionAtT(float t) const
	{
		const PointVector lerpedPoints = CalculateLerpedPoints(mPoints, t);
		assert(lerpedPoints.size() == 1U);

		if (lerpedPoints.size() == 1U)
		{
			return lerpedPoints.at(0);
		}
		else
		{
			return {};
		}
	}

	template<typename CurvePointT, size_t degree>
	CurvePointT BezierCurveSegment<CurvePointT, degree>::CalculatePositionAtDistance(float distance) const
	{
		// TODO
		return {};
	}

	template<typename CurvePointT, size_t degree>
	BezierCurveSegment<CurvePointT, degree>::PointVector BezierCurveSegment<CurvePointT, degree>::CalculateLerpedPoints(const BezierCurveSegment<CurvePointT, degree>::PointVector& inputPoints, float t) const
	{
		CURVELIB_VERIFY_RETURN(inputPoints.size() > 0, {});

		if (inputPoints.size() == 1U)
		{
			// Done! Nothing left to lerp, so return the input by copy
			return inputPoints;
		}
		else
		{
			PointVector lerpedPoints{};
			lerpedPoints.reserve(inputPoints.size() - 1);

			for(size_t i = 0U; i < inputPoints.size() - 1; ++i)
			{
				const CurvePointT lerpedPoint = CurvePointT::Lerp(inputPoints[i], inputPoints[i + 1], t);
				lerpedPoints.push_back(lerpedPoint);
			}

			// Recursively lerp until there's nothing left to lerp between
			return CalculateLerpedPoints(lerpedPoints, t);
		}
	}
}