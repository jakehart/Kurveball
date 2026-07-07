// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include <catch_amalgamated.hpp>

#include "KurveballAll.h"
#include "TestUtils.h"

// Checks that each true element of the axisMask is nonzero, and each false element is zero.
void CheckMatchingAxisMask(const Kurveball::Float3& testVector, const Kurveball::Vector3<bool> axisMask)
{
    using namespace Kurveball;

    CHECK(IsZero(testVector.X) != axisMask.X);
    CHECK(IsZero(testVector.Y) != axisMask.Y);
    CHECK(IsZero(testVector.Z) != axisMask.Z);
}

TEST_CASE("SetVerticalAxis")
{
    // I'm only testing axis masking here. Magnitude and playback behavior are tested in VelocityCurvePlaybackTest.cpp

    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstance = GenerateTestCurveInstance();
    
    // Move equally on all axes so that we can test masking easily
    curveInstance.mMechanic.mDirection.Set(1, 1, 1);
    curveInstance.mMechanic.mDirection.NormalizeInPlace();

    curveInstance.mMechanic.mSpeedMultiplier = 10.f;
    curveInstance.mMechanic.mAxisMode = AxisMode::vertical;
    
    Kurveball::StartVelocityCurve(context, curveInstance);

    // Print the vectors explicitly in the test results if failed
    CAPTURE(context.mOutput.mDirection);
    CAPTURE(context.mOutput.mVelocity);

    SECTION("X is vertical")
    {
        SetVerticalAxis(context, Axis::X);
        TickCurveContext(context, TICK_DURATION, 2);

        // Since we set X to be the vertical axis, and mAxisMode is vertical, only X should contain nonzero output
        CheckMatchingAxisMask(context.mOutput.mDirection, Vector3<bool>(true, false, false));
        CheckMatchingAxisMask(context.mOutput.mVelocity, Vector3<bool>(true, false, false));
    }

    SECTION("Y is vertical")
    {
        SetVerticalAxis(context, Axis::Y);
        TickCurveContext(context, TICK_DURATION, 2);

        CheckMatchingAxisMask(context.mOutput.mDirection, Vector3<bool>(false, true, false));
        CheckMatchingAxisMask(context.mOutput.mVelocity, Vector3<bool>(false, true, false));
    }

    SECTION("Z is vertical")
    {
        SetVerticalAxis(context, Axis::Z);
        TickCurveContext(context, TICK_DURATION, 2);

        CheckMatchingAxisMask(context.mOutput.mDirection, Vector3<bool>(false, false, true));
        CheckMatchingAxisMask(context.mOutput.mVelocity, Vector3<bool>(false, false, true));
    }

}

TEST_CASE("StartVelocityCurve")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstance = GenerateTestCurveInstance();
    curveInstance.mMechanic.mPlayCount = PLAY_COUNT_INFINITE;
    StartVelocityCurve(context, curveInstance);

    REQUIRE(IsCurveRunning(context, curveInstance.mMechanic.mInstanceID));
    // No speed check here, since the curve hasn't ticked yet

    TickCurveContext(context, TICK_DURATION, 2);

    REQUIRE(IsCurveRunning(context, curveInstance.mMechanic.mInstanceID));
    REQUIRE_THAT(context.mOutput.mSpeed, Catch::Matchers::WithinAbs(curveInstance.mMechanic.mSpeedMultiplier, SPEED_TOLERANCE));

    TickCurveContext(context, TICK_DURATION, 100);

    REQUIRE(IsCurveRunning(context, curveInstance.mMechanic.mInstanceID));
    REQUIRE_THAT(context.mOutput.mSpeed, Catch::Matchers::WithinAbs(curveInstance.mMechanic.mSpeedMultiplier, SPEED_TOLERANCE));
}

TEST_CASE("UpdateVelocityCurve")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstance = GenerateTestCurveInstance();
    curveInstance.mMechanic.mPlayCount = PLAY_COUNT_INFINITE;
    curveInstance.mMechanic.mSpeedMultiplier = 10;
    curveInstance.mMechanic.mDirection.Set(1, 0, 0);

    StartVelocityCurve(context, curveInstance);

    SECTION("Sanity check before calling UpdateVelocityCurve")
    {
        TickCurveContext(context, TICK_DURATION, 2);

        REQUIRE_THAT(curveInstance.mMechanic.mSpeedMultiplier, Catch::Matchers::WithinAbs(context.mOutput.mSpeed, SPEED_TOLERANCE));
        REQUIRE(curveInstance.mMechanic.mDirection.Equals(context.mOutput.mDirection));
    }

    SECTION("Ensure that nothing changes when we pass neither speed nor direction")
    {
        UpdateVelocityCurve(context, curveInstance.mMechanic.mInstanceID, std::nullopt, std::nullopt);
        TickCurveContext(context, TICK_DURATION, 2);

        REQUIRE_THAT(curveInstance.mMechanic.mSpeedMultiplier, Catch::Matchers::WithinAbs(context.mOutput.mSpeed, SPEED_TOLERANCE));
        REQUIRE(curveInstance.mMechanic.mDirection.Equals(context.mOutput.mDirection));
    }

    SECTION("Ensure that we can update speed, and that direction stays the same as it was on initialization")
    {
        constexpr float newSpeed = 55.f;
        UpdateVelocityCurve(context, curveInstance.mMechanic.mInstanceID, newSpeed, std::nullopt);
        TickCurveContext(context, TICK_DURATION, 2);

        REQUIRE_THAT(context.mOutput.mSpeed, Catch::Matchers::WithinAbs(newSpeed, SPEED_TOLERANCE));
        REQUIRE(context.mOutput.mDirection.Equals(curveInstance.mMechanic.mDirection));
    }

    SECTION("Ensure that we can update direction, and that speed stays at the value set by initialization")
    {
        const Float3 newDirection = Float3(0, 0, 1);
        UpdateVelocityCurve(context, curveInstance.mMechanic.mInstanceID, std::nullopt, newDirection);
        TickCurveContext(context, TICK_DURATION, 2);

        REQUIRE_THAT(context.mOutput.mSpeed, Catch::Matchers::WithinAbs(curveInstance.mMechanic.mSpeedMultiplier, SPEED_TOLERANCE));
        REQUIRE(context.mOutput.mDirection.Equals(newDirection));
    }
}

TEST_CASE("StopVelocityCurve")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstance = GenerateTestCurveInstance();
    StartVelocityCurve(context, curveInstance);
    REQUIRE(IsCurveRunning(context, curveInstance.mMechanic.mInstanceID));

    StopVelocityCurve(context, curveInstance.mMechanic.mInstanceID);
    REQUIRE(!IsCurveRunning(context, curveInstance.mMechanic.mInstanceID));
}