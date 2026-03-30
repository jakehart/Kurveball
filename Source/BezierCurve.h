// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "BezierCurveSegment.h"
#include "CurveSampler.h"
#include "CurveSampler3D.h"
#include <ostream>
#include <vector>

namespace Kurveball
{
    template<typename PositionT>
    class BezierCurve
    {
    public:
        using CurveSegment = BezierCurveSegment<PositionT>;
        using ScalarType = PositionT::ScalarType;

        BezierCurve() = default;
        BezierCurve(Vector<CurveSegment> segments);

        const auto& GetSegments() const;
        auto& AccessSegments();
        void AddSegment(const CurveSegment& segment);
        void AddSegment(CurveSegment&& segment);

        void ToBinary(std::ostream& outStream) const;
        [[nodiscard]] static BezierCurve FromBinary(std::istream& istream);

        [[nodiscard]] PositionT CalculatePositionAtT(ScalarType t) const;
        [[nodiscard]] PositionT CalculatePositionAtXCoordinate(ScalarType x) const;

        // Regenerates the lookup table of x coordinate to t value for every curve segment in this curve.
        void GenerateXTLookupTables() const;

        [[nodiscard]] CurveSamplerXY CreateCurveSamplerXY() const;
        [[nodiscard]] CurveSampler3D CreateCurveSampler3D() const;

        // Mirrors the tangents of one specific BezierCurveSegment to its neighbors.
        void MirrorTangents(size_t sourceSegmentNum);

    private:
        // TODO: Knot points should not use redundant memory. Can use std::view for this
        Vector<CurveSegment> mSegments{};
    };
}

#include "BezierCurve.inl"