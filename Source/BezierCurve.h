// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "BezierCurveSegment.h"
#include "CurveSampler.h"
#include "CurveSampler3D.h"
#include <ostream>
#include <vector>

namespace CurveLib
{
    template<typename PositionT>
    class BezierCurve
    {
    public:
        using CurveSegment = BezierCurveSegment<PositionT>;
        using ScalarType = PositionT::ScalarType;

        BezierCurve() = default;
        BezierCurve(std::vector<CurveSegment> segments);

        const auto& GetSegments() const;
        auto& AccessSegments();

		void ToBinary(std::ostream& outStream) const;
		[[nodiscard]] static BezierCurve FromBinary(std::istream& istream);

        [[nodiscard]] PositionT CalculatePositionAtT(ScalarType t) const;
        [[nodiscard]] PositionT CalculatePositionAtXCoordinate(ScalarType x) const;

        [[nodiscard]] CurveSamplerXY CreateCurveSamplerXY() const;
        [[nodiscard]] CurveSampler3D CreateCurveSampler3D() const;

    private:
        // TODO: Knot points should not use redundant memory. Can use std::view for this
        std::vector<CurveSegment> mSegments{};
    };
}

#include "BezierCurve.inl"