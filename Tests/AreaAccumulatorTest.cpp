#include <catch_amalgamated.hpp>

#include "AreaAccumulator.h"

#include <cstdint>

TEST_CASE("Integrate a horizontal line")
{
	using namespace CurveLib;

	AreaAccumulator accumulator;
	REQUIRE(accumulator.GetTotalArea() == 0);
	accumulator.Reset();
	REQUIRE(accumulator.GetTotalArea() == 0);

	constexpr float AREA_TOLERANCE = 0.1f;

	for (uint8_t i = 0; i <= 100; ++i)
	{
		accumulator.AccumulateArea(i, 10.f);
	}

	REQUIRE_THAT(accumulator.GetTotalArea(), Catch::Matchers::WithinAbs(1000.f, AREA_TOLERANCE));
}