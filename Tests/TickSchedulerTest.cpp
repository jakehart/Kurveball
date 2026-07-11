#include <catch_amalgamated.hpp>
#include <fstream>
#include <string>
#include "TickScheduler.h"


TEST_CASE("TickScheduler - test number of full and partial ticks, duration of partial tick")
{
    using namespace Kurveball;

    TickScheduler scheduler;

    size_t numFullTicks = 0;
    size_t numPartialTicks = 0;
    Seconds partialTickTime(0.f);

    scheduler.SetFixedSubtickDuration(0.05_sec);

    scheduler.SetFullSubtickCallback([&](Seconds absoluteTime, Seconds fixedSubtickDuration)
        {
            ++numFullTicks;
        });

    scheduler.SetPartialSubtickCallback([&](Seconds absoluteTime, Seconds partialSubtickDuration)
        {
            ++numPartialTicks;
            partialTickTime = partialSubtickDuration;
        });
    
    SECTION("Test tick times less than subtick")
    {
        scheduler.VariableTick(0.025_sec);

        // Should have one partial tick and no full subticks, and the partial tick should be the time we passed in
        REQUIRE(numFullTicks == 0);
        REQUIRE(numPartialTicks == 1);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0.025, 0.001));

        // Tick a few more times using "unpredictable" tick times, without reaching the first full subtick
        scheduler.VariableTick(0.03_sec);
        REQUIRE(numFullTicks == 0);
        REQUIRE(numPartialTicks == 2);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0.03, 0.001));

        scheduler.VariableTick(0.04_sec);
        REQUIRE(numFullTicks == 0);
        REQUIRE(numPartialTicks == 3);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0.04, 0.001));

        scheduler.VariableTick(0.045_sec);
        REQUIRE(numFullTicks == 0);
        REQUIRE(numPartialTicks == 4);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0.045, 0.001));

        // Now tick just after the first subtick and verify that it happens
        scheduler.VariableTick(0.0501_sec);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0.0001, 0.001));
        REQUIRE(numFullTicks == 1);
        REQUIRE(numPartialTicks == 5);
    }

    SECTION("Test one subtick and remainder")
    {
        scheduler.VariableTick(0.051_sec);

        // Should have one partial tick and no full subticks, and the partial tick should be the time we passed in
        REQUIRE(numFullTicks == 1);
        REQUIRE(numPartialTicks == 1);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0.001, 0.001));
    }

    SECTION("Test full subticks only")
    {
        scheduler.VariableTick(0.2_sec);

        // Should have one partial tick and no full subticks, and the partial tick should be the time we passed in
        REQUIRE(numFullTicks == 4);
        REQUIRE(numPartialTicks == 0);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0, sFloatEpsilon));
    }

    SECTION("Test large number of subticks with partial tick")
    {
        scheduler.VariableTick(10.0342_sec);

        REQUIRE(numFullTicks == 200);
        REQUIRE(numPartialTicks == 1);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0.0342, 0.001));

        scheduler.VariableTick(11.502_sec);

        REQUIRE(numFullTicks == 230);
        REQUIRE(numPartialTicks == 2);
        REQUIRE_THAT(partialTickTime.count(), Catch::Matchers::WithinAbs(0.002, 0.001));
    }
}