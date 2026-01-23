// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <optional>
#include <ostream>
#include <vector>
#include "LookupTable.h"

namespace CurveLib
{
    // Implementation of de Casteljau's algorithm to solve a Bezier curve of any degree (up to 3, cubic Bezier,
    // which contains 4 points).
    template<typename PositionT>
    class BezierCurveSegment
    {
    public:
        using PointVector = std::vector<PositionT>;
        using ScalarType = PositionT::ScalarType;
        static const uint8_t MAX_SUPPORTED_POINTS = 4; // Cubic Bezier

        BezierCurveSegment() = default;
        BezierCurveSegment(const PointVector& points);

        void SetPoints(const PointVector& points);
        const std::vector<PositionT>& GetPoints() const;
        std::vector<PositionT>& AccessPoints();

        ScalarType GetStartX() const;
        ScalarType GetEndX() const;

        // Samples the curve by pure T, using De Casteljau's algorithm.
        PositionT CalculatePositionAtT(ScalarType t) const;
        // Uses the arc length to sample the curve instead of t. This produces a consistent, evenly spaced result, whereas
        // sampling by T causes "expansion" and "contraction"
        PositionT CalculatePositionAtDistance(ScalarType distance) const;

        PositionT CalculatePositionAtXCoordinate(ScalarType x) const;

        void ToBinary(std::ostream& outStream) const;
        [[nodiscard]] static BezierCurveSegment FromBinary(std::istream& istream);

        bool HasXTLookupTable() const;
        // Generates a lookup table of x coordinate to t value so that we can index the curve by x (mostly only useful for 2D graphs).
        void GenerateXTLookupTable() const;
        // Returns the number of lookup table samples that this segment has.
        size_t GetLookupSampleRate() const;

        bool IncludesXCoordinate(ScalarType x) const;

    private:
        using PointInfluenceVector = std::vector<ScalarType>;

        // The number of t samples that mLookupTable will have when it's generated. This resolution is per segment.
        static const uint32_t LOOKUP_TABLE_SAMPLE_RATE = 64;

        // Uses Bernstein basis polynomials to calculate the "influence" coefficients of each point in this segment.
        PointInfluenceVector CalculatePointInfluences(ScalarType t) const;

        // The control points of this segment. For example, in the case of cubic Bezier (polynomial degree 3), there are 4 points contained here.
        // TODO: Use std::view on a list of points on the parent BezierCurve
        PointVector mPoints{};

        // Generated on the fly when CalculatePositionAtXCoordinate is called for the first time on this segment, or manually by the user calling GenerateLookupTable.
        mutable std::optional<LookupTable<ScalarType>> mLookupTable{ std::nullopt };
    };
}

#include "BezierCurveSegment.inl"