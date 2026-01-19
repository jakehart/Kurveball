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
    BezierCurveSegment<CurvePointT>::ScalarType BezierCurveSegment<CurvePointT>::GetStartX() const
    {
        CURVELIB_VERIFY_RETURN(!mPoints.empty(), 0);

        return mPoints[0].X;
    }

    template<typename CurvePointT>
    CurvePointT BezierCurveSegment<CurvePointT>::CalculatePositionAtT(ScalarType t) const
    {
        const auto influences = CalculatePointInfluences(t);
        CURVELIB_VERIFY_RETURN(influences.size() == mPoints.size(), {});

        CurvePointT finalPoint;
        for(size_t i = 0; i < mPoints.size(); ++i)
        {
            finalPoint = finalPoint + mPoints[i] * influences[i];
        }
        return finalPoint;
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

        CURVELIB_VERIFY_RETURN(numPoints > 0, {});

        PointVector points;
        for (size_t i = 0; i < numPoints; ++i)
        {
            points.push_back(CurvePointT::FromBinary(istream));
        }

        return BezierCurveSegment(points);
    }

    template<typename CurvePointT>
    BezierCurveSegment<CurvePointT>::PointInfluenceVector BezierCurveSegment<CurvePointT>::CalculatePointInfluences(ScalarType t) const
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