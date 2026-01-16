#include <catch_amalgamated.hpp>

#include "BezierCurveSegment.h"
#include "Vector3.h"

TEST_CASE("CalculatePositionAtT - cubic curve segment that comprises a straight line in 3D")
{
	using namespace CurveLib;

	const float POSITION_AXIS_TOLERANCE = sFloatEpsilon * 4;

	// Cubic Bezier curve with linearly placed, evenly spaced control points. This simplifies away
	// the need to deal with t expansion/contraction when checking the result
	const std::vector<Float3> controlPoints =
	{
		Float3 {0, 0, 0},
		Float3 {1, 0, 0},
		Float3 {2, 0, 0},
		Float3 {3, 0, 0}
	};

	const BezierCurveSegment<Float3> segment(controlPoints);
	
	// Sample the start of the line
	const Float3 sample1 = segment.CalculatePositionAtT(0.f);
	CHECK_THAT(sample1.X, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample1.Y, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample1.Z, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));

	// Sample directly on a control point -- result should match the control point itself exactly (because it's a straight line)
	const Float3 sample2 = segment.CalculatePositionAtT(0.25f);
	CHECK_THAT(sample2.X, Catch::Matchers::WithinAbs(1.f, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample2.Y, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample2.Z, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));

	// Sample 50% between two control points -- the result should be the points' average
	const Float3 sample3 = segment.CalculatePositionAtT(0.375f); // 3/8, which is between point indices 1 and 2
	const Float3 average = (controlPoints[1] + controlPoints[2]) / 2.f;
	CHECK_THAT(sample3.X, Catch::Matchers::WithinAbs(average.X, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample3.Y, Catch::Matchers::WithinAbs(average.Y, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample3.Z, Catch::Matchers::WithinAbs(average.Z, POSITION_AXIS_TOLERANCE));
}