// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "Asserts.h"
#include "LookupTable.h"
#include "MathUtils.h"

namespace CurveLib
{
    template<typename PositionT>
    BezierCurveSegment<PositionT>::BezierCurveSegment(const BezierCurveSegment::PointVector& points)
    {
        // TODO: Complain if SetPoints fails
        SetPoints(points);
    }

    template<typename PositionT>
    void BezierCurveSegment<PositionT>::SetPoints(const PointVector& points)
    {
        mPoints = points;
    }

    template<typename PositionT>
    const std::vector<PositionT>& BezierCurveSegment<PositionT>::GetPoints() const
    {
        return mPoints;
    }

    template<typename PositionT>
    std::vector<PositionT>& BezierCurveSegment<PositionT>::AccessPoints() 
    {
        return mPoints;
    }

    template<typename PositionT>
    BezierCurveSegment<PositionT>::ScalarType BezierCurveSegment<PositionT>::GetStartX() const
    {
        CURVELIB_VERIFY_RETURN(!mPoints.empty(), 0);

        return mPoints.front().X;
    }

	template<typename PositionT>
    BezierCurveSegment<PositionT>::ScalarType BezierCurveSegment<PositionT>::GetEndX() const
    {
		CURVELIB_VERIFY_RETURN(!mPoints.empty(), 0);

        return mPoints.back().X;
	}

    template<typename PositionT>
    PositionT BezierCurveSegment<PositionT>::CalculatePositionAtT(ScalarType t) const
    {
        const auto influences = CalculatePointInfluences(t);
        CURVELIB_VERIFY_RETURN(influences.size() == mPoints.size(), {});

        PositionT finalPoint;
        for(size_t i = 0; i < mPoints.size(); ++i)
        {
            finalPoint = finalPoint + mPoints[i] * influences[i];
        }
        return finalPoint;
    }

    template<typename PositionT>
    PositionT BezierCurveSegment<PositionT>::CalculatePositionAtDistance(ScalarType distance) const
    {
        // TODO : use arc distance calculation
        return {};
    }

	template<typename PositionT>
	PositionT BezierCurveSegment<PositionT>::CalculatePositionAtXCoordinate(ScalarType x) const
    {
        if (!mLookupTable)
        {
            GenerateXTLookupTable();
        }

        CURVELIB_VERIFY_RETURN(mLookupTable, {});

        const ScalarType t = mLookupTable->CalculateValue(x);
        return CalculatePositionAtT(t);
    }

    template<typename PositionT>
    void BezierCurveSegment<PositionT>::ToBinary(std::ostream& outStream) const
    {
        const size_t numPoints = mPoints.size();
        outStream.write((const char*) &numPoints, sizeof(size_t));

        for (const auto point : mPoints)
        {
            point.ToBinary(outStream);
        }

        outStream.flush();
    }

    template<typename PositionT>
    BezierCurveSegment<PositionT> BezierCurveSegment<PositionT>::FromBinary(std::istream& istream)
    {
        size_t numPoints = 0U;
        istream.read((char*)&numPoints, sizeof(size_t));

        CURVELIB_VERIFY_RETURN(numPoints > 0, {});

        PointVector points;
        for (size_t i = 0; i < numPoints; ++i)
        {
            points.push_back(PositionT::FromBinary(istream));
        }

        return BezierCurveSegment(points);
    }

	template<typename PositionT>
	bool BezierCurveSegment<PositionT>::HasXTLookupTable() const
    {
        return mLookupTable.has_value();
    }

	template<typename PositionT>
	void BezierCurveSegment<PositionT>::GenerateXTLookupTable() const
    {
        mLookupTable = LookupTable<ScalarType>(LOOKUP_TABLE_SAMPLE_RATE);

        for (uint32_t sampleNum = 0; sampleNum < LOOKUP_TABLE_SAMPLE_RATE; ++sampleNum)
        {
            const ScalarType t = (ScalarType)sampleNum / LOOKUP_TABLE_SAMPLE_RATE;
            const PositionT position = CalculatePositionAtT(t);
            
            assert(t >= 0 && t <= 1);
            assert(position.X >= mPoints.front().X && position.X <= mPoints.back().X);

            mLookupTable->AddSample(FloorToZero(position.X), FloorToZero(t));
        }
    }

	template<typename PositionT>
    bool BezierCurveSegment<PositionT>::IncludesXCoordinate(ScalarType x) const
    {
        if (!HasXTLookupTable())
        {
            GenerateXTLookupTable();
        }

		CURVELIB_VERIFY_RETURN(mLookupTable, {});

        return x >= mLookupTable->GetMinX() && x <= mLookupTable->GetMaxX();
    }

    template<typename PositionT>
    BezierCurveSegment<PositionT>::PointInfluenceVector BezierCurveSegment<PositionT>::CalculatePointInfluences(ScalarType t) const
    {
        const size_t numPoints = mPoints.size();
        CURVELIB_VERIFY_RETURN(numPoints > 0, {});
        CURVELIB_VERIFY_RETURN(numPoints <= MAX_SUPPORTED_POINTS, {});

        // Which Bernstein polynomials to use depends on how many points we're blending between.
        // I've expressed them in monomial form (already FOILed/simplified), with zeroed terms (like 0t)
        // removed, and with extra parentheses around each term for clarity.
        switch (numPoints)
        {
        case 1:
            return { 1 };
        case 2: // Same as lerp
            return { 1 - t,
                     t };
        case 3:
            return { 1 - (2 * t) + (t * t),
                    (2 * t) - (2 * t * t),
                    (t * t) };
        case 4:
            return { 1 - (3 * t) + (3 * t * t) - (t * t * t),
                    (3 * t) - (6 * t * t) + (3 * t * t * t),
                    (3 * t * t) - (3 * t * t * t),
                    (t * t * t) };

        default:
            return {};
        }
    }
}