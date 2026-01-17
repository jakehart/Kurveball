#include <catch_amalgamated.hpp>

#include <fstream>
#include "BezierCurveSegment.h"
#include "Vector3.h"

TEST_CASE("CalculatePositionAtT - straight line 3D cubic")
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
	CHECK_THAT(sample1.Y, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample1.Z, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));

	// Sample directly on a control point -- result should match the control point itself exactly (because it's a straight line)
	const Float3 sample2 = segment.CalculatePositionAtT(0.25f);
	CHECK_THAT(sample2.Y, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample2.Z, Catch::Matchers::WithinAbs(0, POSITION_AXIS_TOLERANCE));

	// Sample 50% between two control points -- the result should be the points' average
	const Float3 sample3 = segment.CalculatePositionAtT(0.375f); // 3/8, which is between point indices 1 and 2
	const Float3 average = (controlPoints[1] + controlPoints[2]) / 2.f;
	CHECK_THAT(sample3.Y, Catch::Matchers::WithinAbs(average.Y, POSITION_AXIS_TOLERANCE));
	CHECK_THAT(sample3.Z, Catch::Matchers::WithinAbs(average.Z, POSITION_AXIS_TOLERANCE));
}

TEST_CASE("Binary de/serialize")
{
	using namespace CurveLib;

	const std::vector<Float3> originalPoints =
	{
		Float3 {0.1234f, 456.678f, -0.05f},
		Float3 {1.15f, -792.5234f, -10.f},
		Float3 {2213.f, -555.5f, 5246.3465f},
		Float3 {-836754.f, 4893.345f, -453.869f}
	};

	const BezierCurveSegment<Float3> originalSegment(originalPoints);

	const std::string TEST_FILE_NAME = "BezierSegmentTestFile.bin";
	std::ofstream outStream(TEST_FILE_NAME);
	originalSegment.ToBinary(outStream);
	outStream.close();

	std::ifstream inStream(TEST_FILE_NAME);
	const auto serializedSegment = BezierCurveSegment<Float3>::FromBinary(inStream);
	inStream.close();

	const auto& serializedPoints = serializedSegment.GetPoints();
	REQUIRE(serializedPoints.size() == originalPoints.size());

	for(size_t i = 0; i < serializedPoints.size(); ++i)
	{
		const auto& originalPoint = originalPoints[i];
		const auto& serializedPoint = serializedPoints[i];

		REQUIRE_THAT(serializedPoint.X, Catch::Matchers::WithinAbs(originalPoint.X, sFloatEpsilon));
		REQUIRE_THAT(serializedPoint.Y, Catch::Matchers::WithinAbs(originalPoint.Y, sFloatEpsilon));
		REQUIRE_THAT(serializedPoint.Z, Catch::Matchers::WithinAbs(originalPoint.Z, sFloatEpsilon));
	}
}