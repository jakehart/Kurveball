// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <algorithm>

#include "BezierCurve.h"
#include "Asserts.h"

namespace CurveLib
{
	template<typename PositionT>
	BezierCurve<PositionT>::BezierCurve(std::vector<CurveSegment> segments)
        : mSegments(segments)
	{
	}


	template<typename PositionT>
	const auto& BezierCurve<PositionT>::GetSegments() const
	{
        return mSegments;
	}

	template<typename PositionT>
	auto& BezierCurve<PositionT>::AccessSegments()
    {
        return mSegments;
    }

    template<typename PositionT>
    PositionT BezierCurve<PositionT>::CalculatePositionAtT(float t)
    {
        // Regardless of the characteristics of each segment, its t always has a range of 1.
        // For example, the first segment has t=[0, 1], the second has t=[1, 2], etc.
        const size_t segmentIndex = std::floor(t);
        if (segmentIndex >= mSegments.size())
        {
            // This t is outside the curve
            return {};
        }

        return mSegments.at(segmentIndex).CalculatePositionAtT(t - segmentIndex);
    }

	template<typename PositionT>
    PositionT CurveLib::BezierCurve<PositionT>::CalculatePositionAtXCoordinate(float x)
    {
        const auto segmentIter = std::lower_bound(mSegments.begin(), mSegments.end(), x,
                                                  [](const BezierCurveSegment<PositionT>& segment, ScalarType findX)
                                                  {
                                                  });

        if (segmentIter == mSegments.end())
        {
            return {};
        }

        return segmentIter->CalculatePositionAtXCoordinate(x);
    }

    template<typename PositionT>
    CurveSamplerXY CurveLib::BezierCurve<PositionT>::CreateCurveSamplerXY() const
    {
        return [this](float) -> float
            {
                // TODO: need to be able to sample by x
                return -999;
            };
    }

    template<typename PositionT>
    CurveSampler3D CurveLib::BezierCurve<PositionT>::CreateCurveSampler3D() const
    {
        return [this](float) -> float
            {
                // TODO: need to be able to sample by arc distance
                return {};
            };
    }
}