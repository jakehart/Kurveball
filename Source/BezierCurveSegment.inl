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
	void BezierCurveSegment<CurvePointT>::ToBinary(std::ostream& outStream) const
	{
		const size_t numPoints = mPoints.size();
		outStream.write((const char*) &numPoints, sizeof(size_t));

		for (const auto point : mPoints)
		{
			point.ToBinary(outStream);
		}

		outStream.flush();
	}

	template<typename CurvePointT>
	BezierCurveSegment<CurvePointT> BezierCurveSegment<CurvePointT>::FromBinary(std::istream& istream)
	{
		size_t numPoints = 0U;
		istream.read((char*)&numPoints, sizeof(size_t));

		CURVELIB_VERIFY_RETURN(numPoints > 0U, {});

		PointVector points;
		for (size_t i = 0; i < numPoints; ++i)
		{
			points.push_back(CurvePointT::FromBinary(istream));
		}

		return BezierCurveSegment(points);
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