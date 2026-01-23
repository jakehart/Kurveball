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
	void BezierCurve<PositionT>::AddSegment(const CurveSegment& segment)
    {
        mSegments.push_back(segment);
    }

	template<typename PositionT>
    void BezierCurve<PositionT>::AddSegment(CurveSegment&& segment)
    {
		mSegments.push_back(segment);
    }

	template<typename PositionT>
	void BezierCurve<PositionT>::ToBinary(std::ostream& outStream) const
	{
		const size_t numSegments = mSegments.size();
		outStream.write((const char*)&numSegments, sizeof(size_t));

		for (const auto segment : mSegments)
		{
			segment.ToBinary(outStream);
		}

		outStream.flush();
	}

	template<typename PositionT>
    BezierCurve<PositionT> BezierCurve<PositionT>::FromBinary(std::istream& inStream)
	{
		size_t numSegments = 0U;
        inStream.read((char*)&numSegments, sizeof(size_t));

		CURVELIB_VERIFY_RETURN(numSegments > 0, {});

        std::vector<BezierCurveSegment<PositionT>> segments{};
        segments.reserve(numSegments);

		for (size_t i = 0; i < numSegments && inStream.good() && !inStream.eof(); ++i)
		{
			segments.push_back(BezierCurveSegment<PositionT>::FromBinary(inStream));
		}

        return BezierCurve(segments);
	}

    template<typename PositionT>
    PositionT BezierCurve<PositionT>::CalculatePositionAtT(ScalarType t) const
    {
        // Regardless of the characteristics of each segment, its t always has a range of 1.
        // For example, the first segment has t=[0, 1], the second has t=[1, 2], etc.
        const size_t segmentIndex = (size_t)std::floor(t);
        if (segmentIndex >= mSegments.size())
        {
            // This t is outside the curve
            return {};
        }

        return mSegments.at(segmentIndex).CalculatePositionAtT(t - segmentIndex);
    }

    template<typename PositionT>
    PositionT CurveLib::BezierCurve<PositionT>::CalculatePositionAtXCoordinate(ScalarType x) const
    {
        // Make sure all the tables are generated before we try to query them.
        GenerateXTLookupTables();

        const auto segmentIter = std::lower_bound(mSegments.begin(), mSegments.end(), x,
                                                  [](const BezierCurveSegment<PositionT>& segment, ScalarType findX)
                                                  {
                                                      return segment.IncludesXCoordinate(findX);
                                                  });

        if (segmentIter == mSegments.end())
        {
            return {};
        }

        return segmentIter->CalculatePositionAtXCoordinate(x);
    }

	template<typename PositionT>
    void BezierCurve<PositionT>::GenerateXTLookupTables() const
    {
        // TODO: Only generate segments that are dirty.
        for (const auto& segment : mSegments)
        {
            segment.GenerateXTLookupTable();
        }
    }

    template<typename PositionT>
    CurveSamplerXY BezierCurve<PositionT>::CreateCurveSamplerXY() const
    {
        return [this](ScalarType x) -> ScalarType
            {
                return CalculatePositionAtXCoordinate(x);
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