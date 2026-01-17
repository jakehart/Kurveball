// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "MathUtils.h"
#include "Asserts.h"

namespace CurveLib
{
	template<typename CurvePointT>
	BezierCurveSegment<CurvePointT>::BezierCurveSegment(const BezierCurveSegment::PointVector& points)
	{
		// TODO: Complain if SetPoints fails
		SetPoints(points);
	}

	template<typename CurvePointT>
	void BezierCurveSegment<CurvePointT>::SetPoints(const PointVector& points)
	{
		mPoints = points;
	}

	template<typename CurvePointT>
	const std::vector<CurvePointT>& BezierCurveSegment<CurvePointT>::GetPoints() const
	{
		return mPoints;
	}

	template<typename CurvePointT>
	std::vector<CurvePointT>& BezierCurveSegment<CurvePointT>::AccessPoints() 
	{
		return mPoints;
	}

	template<typename CurvePointT>
	CurvePointT BezierCurveSegment<CurvePointT>::CalculatePositionAtT(ScalarType t) const
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

	template<typename CurvePointT>
	CurvePointT BezierCurveSegment<CurvePointT>::CalculatePositionAtDistance(ScalarType distance) const
	{
		// TODO
		return {};
	}

	template<typename CurvePointT>
	BezierCurveSegment<CurvePointT>::PointVector BezierCurveSegment<CurvePointT>::CalculateLerpedPoints(const BezierCurveSegment<CurvePointT>::PointVector& inputPoints, ScalarType t) const
	{
		CURVELIB_VERIFY_RETURN(inputPoints.size() > 0, {});

		if (inputPoints.size() == 1U)
		{
			// Done! Nothing left to lerp, so return the input points by copy
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