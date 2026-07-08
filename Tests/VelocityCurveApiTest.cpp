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

    auto curveInstanceToStop = GenerateTestCurveInstance();
    StartVelocityCurve(context, curveInstanceToStop);
    REQUIRE(IsCurveRunning(context, curveInstanceToStop.mMechanic.mInstanceID));

    auto curveInstanceToKeep = GenerateTestCurveInstance();
    StartVelocityCurve(context, curveInstanceToKeep);
    REQUIRE(IsCurveRunning(context, curveInstanceToKeep.mMechanic.mInstanceID));

    StopVelocityCurve(context, curveInstanceToStop.mMechanic.mInstanceID);
    REQUIRE(!IsCurveRunning(context, curveInstanceToStop.mMechanic.mInstanceID));
    REQUIRE(IsCurveRunning(context, curveInstanceToKeep.mMechanic.mInstanceID));
}

TEST_CASE("StopAllVelocityCurves")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstance1 = GenerateTestCurveInstance();
    auto curveInstance2 = GenerateTestCurveInstance();
    auto curveInstance3 = GenerateTestCurveInstance();
    
    StartVelocityCurve(context, curveInstance1);
    StartVelocityCurve(context, curveInstance2);
    StartVelocityCurve(context, curveInstance3);
    
    REQUIRE(IsCurveRunning(context, curveInstance1.mMechanic.mInstanceID));
    REQUIRE(IsCurveRunning(context, curveInstance2.mMechanic.mInstanceID));
    REQUIRE(IsCurveRunning(context, curveInstance3.mMechanic.mInstanceID));

    StopAllVelocityCurves(context);

    REQUIRE(!IsAnyCurveRunning(context));
}

TEST_CASE("SoftStopVelocityCurve")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstanceInput = GenerateTestCurveInstance();
    curveInstanceInput.mMechanic.mLoopStartX = 0;
    curveInstanceInput.mMechanic.mLoopEndX = 0.75f;
    curveInstanceInput.mMechanic.mPlayCount = 0; // Infinite replays
    curveInstanceInput.mMechanic.mRawAssetDuration = Seconds(1.f);
    curveInstanceInput.mMechanic.mStretchDuration = Seconds(0.f); // Unstretched

    StartVelocityCurve(context, curveInstanceInput);
    TickCurveContext(context, TICK_DURATION, 2);

    // Since this is a const ref, it updates automatically as the curve runs
    const VelocityCurveInstance* curveInstanceInternal = GetCurveInstance(context, curveInstanceInput.mMechanic.mInstanceID);
    REQUIRE(curveInstanceInternal != nullptr);
    const VelocityCurveOutput& curveOutput = curveInstanceInternal->mOutput;

    REQUIRE_THAT(CalculateCurveX(context, curveInstanceInput.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(TICK_DURATION.count() * 2.f, TIME_TOLERANCE_SHORT.count()));

    REQUIRE(IsCurveRunning(context, curveInstanceInput.mMechanic.mInstanceID));

    SoftStopVelocityCurve(context, curveInstanceInput.mMechanic.mInstanceID);
    
    REQUIRE(curveInstanceInternal->mMechanic.mPlayCount == 1);

    REQUIRE_THAT(Internal::CalculateCurveX(*curveInstanceInternal, context.mAbsoluteTime), Catch::Matchers::WithinAbs(0.75f, TIME_TOLERANCE_SHORT.count()));

    // Soft stop isn't immediate, so the curve should still be running, just seeked to the outro
    REQUIRE(IsCurveRunning(context, curveInstanceInput.mMechanic.mInstanceID));

    // Tick once to make sure that playback continues correctly after the seek
    TickCurveContext(context, TICK_DURATION, 1);

    // Verify that we're at the outro section plus one tick
    REQUIRE_THAT(CalculateCurveX(context, curveInstanceInput.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.75f + TICK_DURATION.count(), TIME_TOLERANCE_SHORT.count()));

    // Since SoftStop seeked to the loop end at 0.75, three more ticks of 0.1sec each are enough for the curve to end
    TickCurveContext(context, TICK_DURATION, 3);

    REQUIRE(!IsCurveRunning(context, curveInstanceInput.mMechanic.mInstanceID));
    REQUIRE(!IsAnyCurveRunning(context));
}

TEST_CASE("SeekToX")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstance = GenerateTestCurveInstance();
    curveInstance.mMechanic.mLoopStartX = 0.0f;
    curveInstance.mMechanic.mLoopEndX = 0.0f; // Play the full curve

    StartVelocityCurve(context, curveInstance);

    SeekToX(context, curveInstance.mMechanic.mInstanceID, 0.5f);
    REQUIRE_THAT(CalculateCurveX(context, curveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.5f, TIME_TOLERANCE_SHORT.count()));

    // Verify that playback continues correctly after seek
    TickCurveContext(context, TICK_DURATION, 1);
    CAPTURE(CalculateCurveX(context, curveInstance.mMechanic.mInstanceID));
    REQUIRE_THAT(CalculateCurveX(context, curveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.5f + TICK_DURATION.count(), TIME_TOLERANCE_SHORT.count()));

    // Start over and test it with loop points
    StopVelocityCurve(context, curveInstance.mMechanic.mInstanceID);
    curveInstance.mMechanic.mLoopStartX = 0.3f;
    curveInstance.mMechanic.mLoopEndX = 0.77f;
    curveInstance.mMechanic.mPlayCount = 55;
    StartVelocityCurve(context, curveInstance);
    
    // Seek to somewhere before the loop point
    SeekToX(context, curveInstance.mMechanic.mInstanceID, 0.1f);
    REQUIRE_THAT(CalculateCurveX(context, curveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.1f, TIME_TOLERANCE_SHORT.count()));

    // Seek to inside the looped section
    SeekToX(context, curveInstance.mMechanic.mInstanceID, 0.44f);
    REQUIRE_THAT(CalculateCurveX(context, curveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.44f, TIME_TOLERANCE_SHORT.count()));

    // Seek to after the looped section
    SeekToX(context, curveInstance.mMechanic.mInstanceID, 0.99f);
    REQUIRE_THAT(CalculateCurveX(context, curveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.99f, TIME_TOLERANCE_SHORT.count()));

    // Tick so that the curve ends
    TickCurveContext(context, TICK_DURATION, 1);

    REQUIRE(!IsCurveRunning(context, curveInstance.mMechanic.mInstanceID));
    REQUIRE(!IsAnyCurveRunning(context));
}

TEST_CASE("GetMechanicDirection")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    Vector3 testDirection(0.123f, 0.456f, 0.789f);
    testDirection.NormalizeInPlace();

    auto curveInstance = GenerateTestCurveInstance();
    curveInstance.mMechanic.mDirection.Set(testDirection.X, testDirection.Y, testDirection.Z);

    StartVelocityCurve(context, curveInstance);
    TickCurveContext(context, TICK_DURATION, 1);

    REQUIRE(GetMechanicDirection(context, curveInstance.mMechanic.mInstanceID).Equals(testDirection));
}

TEST_CASE("GetMechanicSpeedOutput")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstance = GenerateTestCurveInstance();
    curveInstance.mMechanic.mSpeedMultiplier = 333.3f;

    StartVelocityCurve(context, curveInstance);
    TickCurveContext(context, TICK_DURATION, 1);

    // Since the test curve always returns 1, we should get the speed multiplier back
    REQUIRE_THAT(GetMechanicSpeedOutput(context, curveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(curveInstance.mMechanic.mSpeedMultiplier, SPEED_TOLERANCE));
}

TEST_CASE("GetTotalSpeed")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto curveInstance1 = GenerateTestCurveInstance();
    curveInstance1.mMechanic.mSpeedMultiplier = 100.0f;
    curveInstance1.mMechanic.mDirection.Set(1, 0, 0);
    StartVelocityCurve(context, curveInstance1);

    // Start another curve going the opposite direction
    auto curveInstance2 = GenerateTestCurveInstance();
    curveInstance2.mMechanic.mSpeedMultiplier = 20.0f;
    curveInstance2.mMechanic.mDirection.Set(-1, 0, 0);
    StartVelocityCurve(context, curveInstance2);

    // Since going opposite ways, expect speed to be the difference between the two curves' speed multipliers
    TickCurveContext(context, TICK_DURATION, 2);
    REQUIRE_THAT(GetTotalSpeed(context), Catch::Matchers::WithinAbs(80.f, SPEED_TOLERANCE));

    // Set curve 2 to go in the same direction as curve 1
    UpdateVelocityCurve(context, curveInstance2.mMechanic.mInstanceID, std::nullopt, curveInstance1.mMechanic.mDirection);

    // Speed is now the sum of the speed multipliers
    TickCurveContext(context, TICK_DURATION, 2);
    REQUIRE_THAT(GetTotalSpeed(context), Catch::Matchers::WithinAbs(120.f, SPEED_TOLERANCE));
}

TEST_CASE("ResetContext")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    StartVelocityCurve(context, GenerateTestCurveInstance());
    StartVelocityCurve(context, GenerateTestCurveInstance());
    StartVelocityCurve(context, GenerateTestCurveInstance());

    ResetContext(context);

    REQUIRE(!IsAnyCurveRunning(context));
}

TEST_CASE("TransferCurve")
{
    using namespace Kurveball;
    VelocityCurveContext context;

    auto sourceCurveInstance = GenerateTestCurveInstance();
    sourceCurveInstance.mSpeedSampler = [](float curveX)
        {
            // Straight line, y=x
            return curveX;
        };
    sourceCurveInstance.mMechanic.mRawAssetDuration = Seconds(1.f);
    sourceCurveInstance.mMechanic.mSpeedMultiplier = 1.f;
    StartVelocityCurve(context, sourceCurveInstance);

    // Tick 30% of the way through the source curve. We expect speed to be 0.3
    TickCurveContext(context, Seconds(0.1f), 3);

    REQUIRE_THAT(GetMechanicSpeedOutput(context, sourceCurveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.3f, SPEED_TOLERANCE));

    auto targetCurveInstance = GenerateTestCurveInstance();
    targetCurveInstance.mSpeedSampler = [](float curveX)
        {
            // x-flipped line
            return 1 - curveX;
        };
    targetCurveInstance.mMechanic.mRawAssetDuration = Seconds(1.f);
    targetCurveInstance.mMechanic.mSpeedMultiplier = 1.f;
    // No need to start targetCurveInstance, since TransferCurve() is able to do so

    SECTION("Hard cut")
    {
        TransferCurve(context, sourceCurveInstance, targetCurveInstance, BlendType::Cut, Seconds(0.f), true);

        REQUIRE(!IsCurveRunning(context, sourceCurveInstance.mMechanic.mInstanceID));
        REQUIRE(IsCurveRunning(context, targetCurveInstance.mMechanic.mInstanceID));
        // Since the target curve is x-flipped from the source curve, its x should be at 70%
        REQUIRE_THAT(CalculateCurveX(context, targetCurveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.7f, SPEED_TOLERANCE));

        // Do a tiny tick just to update the speed output without significantly advancing the curve
        TickCurveContext(context, Seconds(0.00001f), 1);

        REQUIRE_THAT(GetMechanicSpeedOutput(context, targetCurveInstance.mMechanic.mInstanceID), Catch::Matchers::WithinAbs(0.3f, SPEED_TOLERANCE));
    }

}