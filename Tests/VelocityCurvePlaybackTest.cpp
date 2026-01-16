#include <catch_amalgamated.hpp>

#include "AxisMode.h"
#include "CoordinateSpace.h"
#include "Vector3.h"
#include "VelocityCurveApi.h"
#include "VelocityCurveContext.h"
#include "VelocityCurveInstance.h"
#include "VelocityCurveOutput.h"
#include "VelocityCurvePlayback.h"

#include "TestUtils.h"

const CurveLib::Seconds TICK_DURATION(0.1);
const CurveLib::Seconds TIME_TOLERANCE = TICK_DURATION * 2.f;
const CurveLib::Seconds TICK_DURATION_SHORT(0.05);
const CurveLib::Seconds TIME_TOLERANCE_SHORT = TICK_DURATION_SHORT * 2.f;

const float DEGREE_TOLERANCE = 1.f;
const float DISTANCE_TOLERANCE = 0.5f;

TEST_CASE("TickPlayback with local direction")
{
	constexpr size_t NUM_TICKS = 10;
	using namespace CurveLib;
	VelocityCurveContext context;

	// Give the entity a 90 degree yaw
	CurveLib::SetVerticalAxis(context, Axis::Z);
	CurveLib::SetRotation(context, 0, 0, 90);

	auto curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mDirection.Set(1, 0, 0);
	curveInstance.mMechanic.mCoordinateSpace = CoordinateSpace::local;

	CurveLib::StartVelocityCurve(context, curveInstance);

	TickCurveContext(context, TICK_DURATION, NUM_TICKS);

	// The world-space output direction should be rotated 90 deg relative to the input.
	REQUIRE_THAT(context.mOutput.mDirection.X, Catch::Matchers::WithinAbs(0, DISTANCE_TOLERANCE));
	REQUIRE_THAT(context.mOutput.mDirection.Y, Catch::Matchers::WithinAbs(1, DISTANCE_TOLERANCE));
	REQUIRE_THAT(context.mOutput.mDirection.Z, Catch::Matchers::WithinAbs(0, DISTANCE_TOLERANCE));
}

TEST_CASE("TickPlayback with negative curve output")
{
	constexpr size_t NUM_TICKS = 100;
	using namespace CurveLib;
	VelocityCurveContext context;

	auto curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mSpeedMultiplier = 10.f;
	curveInstance.mMechanic.mDirection.Set(1, 0, 0);
	curveInstance.mMechanic.mAxisMode = AxisMode::allMovementAxes;
	
	curveInstance.mSpeedSampler = [](float x) -> float
		{
			return -1.f;
		};

	CurveLib::StartVelocityCurve(context, curveInstance);
	TickCurveContext(context, TICK_DURATION, NUM_TICKS);

	const float expectedX = -1 * curveInstance.mMechanic.mSpeedMultiplier * TICK_DURATION.count() * (NUM_TICKS-1);
	REQUIRE_THAT(context.mOutput.mPosition.X, Catch::Matchers::WithinAbs(expectedX, DISTANCE_TOLERANCE));
}

TEST_CASE("TickPlayback with rotation - wide ticks")
{
	constexpr size_t NUM_TICKS = 10;
	using namespace CurveLib;
	VelocityCurveContext context;

	auto curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mAxisMode = AxisMode::yaw;
	curveInstance.mMechanic.mSpeedMultiplier = 10.f; // Degrees per second

	CurveLib::StartVelocityCurve(context, curveInstance);
	TickCurveContext(context, TICK_DURATION, NUM_TICKS);
	float expectedYawDegrees = curveInstance.mMechanic.mSpeedMultiplier * (NUM_TICKS - 1) * TICK_DURATION.count();

	REQUIRE_THAT(context.mOutput.mRotation.X, Catch::Matchers::WithinAbs(0, DEGREE_TOLERANCE));
	REQUIRE_THAT(context.mOutput.mRotation.Y, Catch::Matchers::WithinAbs(0, DEGREE_TOLERANCE));
	REQUIRE_THAT(context.mOutput.mRotation.Z, Catch::Matchers::WithinAbs(expectedYawDegrees, DEGREE_TOLERANCE));
}

TEST_CASE("TickPlayback with rotation - short ticks")
{
	constexpr size_t NUM_TICKS = 100;
	using namespace CurveLib;
	VelocityCurveContext context;

	auto curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mAxisMode = AxisMode::yaw;
	curveInstance.mMechanic.mSpeedMultiplier = 10.f; // Degrees per second

	CurveLib::StartVelocityCurve(context, curveInstance);
	TickCurveContext(context, TICK_DURATION_SHORT, NUM_TICKS);
	float expectedYawDegrees = fmodf(curveInstance.mMechanic.mSpeedMultiplier * (NUM_TICKS - 1) * TICK_DURATION_SHORT.count(), 360.f);

	REQUIRE_THAT(context.mOutput.mRotation.X, Catch::Matchers::WithinAbs(0, DEGREE_TOLERANCE));
	REQUIRE_THAT(context.mOutput.mRotation.Y, Catch::Matchers::WithinAbs(0, DEGREE_TOLERANCE));
	REQUIRE_THAT(context.mOutput.mRotation.Z, Catch::Matchers::WithinAbs(expectedYawDegrees, DEGREE_TOLERANCE));
}

TEST_CASE("TickPlayback with stretch duration")
{
	using namespace CurveLib;

	VelocityCurveContext context;

	auto curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mSpeedMultiplier = 1.f;
	curveInstance.mMechanic.mPlayCount = 1; // So we can easily verify the curve end time
	curveInstance.mMechanic.mRawAssetDuration = Seconds(1);
	curveInstance.mMechanic.mStretchDuration = Seconds(0.1);

	{
		curveInstance.mMechanic.mStretchDuration = Seconds(0.1);
		const auto actualEndTime = RunCurveAndGetEndTime(context, curveInstance, TICK_DURATION_SHORT);
		REQUIRE_THAT(actualEndTime.count(), Catch::Matchers::WithinAbs(curveInstance.mMechanic.mStretchDuration.count(), TIME_TOLERANCE_SHORT.count()));
	}

	{
		curveInstance.mMechanic.mStretchDuration = Seconds(1);
		const auto actualEndTime = RunCurveAndGetEndTime(context, curveInstance, TICK_DURATION_SHORT);
		REQUIRE_THAT(actualEndTime.count(), Catch::Matchers::WithinAbs(curveInstance.mMechanic.mStretchDuration.count(), TIME_TOLERANCE_SHORT.count()));
	}

	{
		curveInstance.mMechanic.mStretchDuration = Seconds(2);
		const auto actualEndTime = RunCurveAndGetEndTime(context, curveInstance, TICK_DURATION_SHORT);
		REQUIRE_THAT(actualEndTime.count(), Catch::Matchers::WithinAbs(curveInstance.mMechanic.mStretchDuration.count(), TIME_TOLERANCE_SHORT.count()));
	}

	{
		curveInstance.mMechanic.mStretchDuration = Seconds(5);
		const auto actualEndTime = RunCurveAndGetEndTime(context, curveInstance, TICK_DURATION_SHORT);
		REQUIRE_THAT(actualEndTime.count(), Catch::Matchers::WithinAbs(curveInstance.mMechanic.mStretchDuration.count(), TIME_TOLERANCE_SHORT.count()));
	}
}

TEST_CASE("TickPlayback with stretch duration and loop points")
{
	using namespace CurveLib;

	VelocityCurveContext context;

	auto curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mSpeedMultiplier = 1.f;
	
	// This means the looped portion is played a total of 3 times. The intro (before mLoopStartX)
	// and outro (after mLoopEndX) are each played once
	curveInstance.mMechanic.mPlayCount = 3;
	curveInstance.mMechanic.mRawAssetDuration = Seconds(1);
	// This is total duration to stretch the asset to, assuming it loops once.
	curveInstance.mMechanic.mStretchDuration = Seconds(10.f);
	// The middle half of the curve loops
	curveInstance.mMechanic.mLoopStartX = 0.25f;
	curveInstance.mMechanic.mLoopEndX = 0.75f;

	const Seconds loopingPortionRawLength(curveInstance.mMechanic.mLoopEndX - curveInstance.mMechanic.mLoopStartX);

	{
		const Seconds actualEndTime = RunCurveAndGetEndTime(context, curveInstance, TICK_DURATION);
		
		// Stretch the asset to the mStretchDuration, THEN apply looping.
		const Seconds introDuration(curveInstance.mMechanic.mLoopStartX * curveInstance.mMechanic.mStretchDuration);
		const Seconds outroDuration((curveInstance.mMechanic.mRawAssetDuration.count() - curveInstance.mMechanic.mLoopEndX) * curveInstance.mMechanic.mStretchDuration);
		const Seconds loopDuration(loopingPortionRawLength * curveInstance.mMechanic.mPlayCount * curveInstance.mMechanic.mStretchDuration.count());
		const Seconds expectedEndTime = introDuration + loopDuration + outroDuration;
		
		REQUIRE_THAT(actualEndTime.count(), Catch::Matchers::WithinAbs(expectedEndTime.count(), TIME_TOLERANCE.count()));
	}
}

TEST_CASE("TickPlayback rotation")
{
	using namespace CurveLib;

	VelocityCurveContext context;

	auto curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mSpeedMultiplier = 1.f; // degrees per second
	curveInstance.mMechanic.mAxisMode = AxisMode::yaw;
	curveInstance.mMechanic.mPlayCount = CurveLib::PLAY_COUNT_INFINITE;

	StartVelocityCurve(context, curveInstance);
	REQUIRE(IsCurveRunning(context, curveInstance.mMechanic.mInstanceId));
	REQUIRE(context.mLinearCurves.size() == 0);
	REQUIRE(context.mRotationCurves.size() == 1);

	Float3 expectedPosition{};
	for (uint8_t i = 0; i < 100; ++i)
	{
		context.mAbsoluteTime = Seconds(i * 1.f);
		CurveLib::TickPlayback(context, context.mAbsoluteTime);

		if (i > 0)
		{
			REQUIRE(context.mOutput.mHasEverUpdated);
			// Rotations are hardcoded to Unreal axes for now, so yaw is around the Z axis.
			REQUIRE_THAT(context.mOutput.mRotation.X, Catch::Matchers::WithinRel(0.f));
			REQUIRE_THAT(context.mOutput.mRotation.Y, Catch::Matchers::WithinRel(0.f));
			CHECK_THAT(context.mOutput.mRotation.Z, Catch::Matchers::WithinRel(curveInstance.mMechanic.mSpeedMultiplier * context.mAbsoluteTime.count()));
		}

		// Ensure no translation occurred
		REQUIRE_THAT(context.mOutput.mSpeed, Catch::Matchers::WithinRel(0.f));
		REQUIRE_THAT(context.mOutput.mVelocity.GetLength(), Catch::Matchers::WithinRel(0.f));
		REQUIRE_THAT(context.mOutput.mPosition.GetLength(), Catch::Matchers::WithinRel(0.f));
		REQUIRE_THAT(context.mOutput.mPositionDelta.GetLength(), Catch::Matchers::WithinRel(0.f));
	}
}

TEST_CASE("TickPlayback with starting position (position drift check)")
{
	using namespace CurveLib;

	VelocityCurveContext context;
	CurveLib::SetPosition(context, 10, 10, 10);

	VelocityCurveInstance curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mPlayCount = CurveLib::PLAY_COUNT_INFINITE;
	curveInstance.mMechanic.mRawAssetDuration = Seconds(0.5);
	curveInstance.mMechanic.mStretchDuration = Seconds(0.0); // No stretch (use asset duration)
	curveInstance.mMechanic.mSpeedMultiplier = 0.f; // Go nowhere so that any position error is isolated

	CurveLib::StartVelocityCurve(context, curveInstance);
	TickPlayback(context, context.mAbsoluteTime);
	TickPlayback(context, context.mAbsoluteTime);
	TickPlayback(context, context.mAbsoluteTime);

	REQUIRE_THAT(context.mOutput.mPosition.X, Catch::Matchers::WithinAbs(10, CurveLib::sFloatEpsilon));
	REQUIRE_THAT(context.mOutput.mPosition.Y, Catch::Matchers::WithinAbs(10, CurveLib::sFloatEpsilon));
	REQUIRE_THAT(context.mOutput.mPosition.Z, Catch::Matchers::WithinAbs(10, CurveLib::sFloatEpsilon));
}

TEST_CASE("VelocityCurveInstance::mDistanceAccumulator data continuity during TickSingleCurve")
{
	using namespace CurveLib;

	VelocityCurveContext context;
	VelocityCurveInstance curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mSpeedMultiplier = 5.f;
	
	REQUIRE(!curveInstance.mDistanceAccumulator.HasEverUpdated());
	REQUIRE(curveInstance.mDistanceAccumulator.GetTotalArea() < sFloatEpsilon);

	float previousArea = 0.f;
	for (uint8_t i = 0; i < 100; ++i)
	{
		context.mAbsoluteTime = i * TICK_DURATION;
		CurveLib::Internal::TickSingleCurve(curveInstance, context);
		REQUIRE(curveInstance.mDistanceAccumulator.HasEverUpdated());
		REQUIRE(curveInstance.mOutput.mHasEverUpdated);

		const float totalArea = curveInstance.mDistanceAccumulator.GetTotalArea();
		if (i > 0)
		{
			REQUIRE(totalArea > CurveLib::sFloatEpsilon);
			REQUIRE(totalArea > previousArea);
		}
		previousArea = totalArea;
	}
}

TEST_CASE("VelocityCurveInstance::mDistanceAccumulator data continuity during TickPlayback")
{
	using namespace CurveLib;

	VelocityCurveContext context;
	VelocityCurveInstance curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mSpeedMultiplier = 5.f;
	curveInstance.mMechanic.mPlayCount = CurveLib::PLAY_COUNT_INFINITE;

	StartVelocityCurve(context, curveInstance);
	REQUIRE(CurveLib::IsCurveRunning(context, curveInstance.mMechanic.mInstanceId));

	float previousArea = 0.f;
	for (uint8_t i = 0; i < 100; ++i)
	{
		context.mAbsoluteTime = i * TICK_DURATION;
		CurveLib::TickPlayback(context, context.mAbsoluteTime);

		const auto& accumulator = context.mLinearCurves.find(curveInstance.mMechanic.mInstanceId)->second.mDistanceAccumulator;
		REQUIRE(accumulator.HasEverUpdated());

		const float currentArea = accumulator.GetTotalArea();
		if (i > 0)
		{
			REQUIRE(currentArea > CurveLib::sFloatEpsilon);
			REQUIRE(currentArea > previousArea);
		}
		previousArea = currentArea;
	}
}

TEST_CASE("CombineCurveOutput")
{
	using namespace CurveLib;

	VelocityCurveContext context;
	VelocityCurveInstance instance1 = GenerateTestCurveInstance();
	VelocityCurveInstance instance2 = GenerateTestCurveInstance();
	instance2.mMechanic.mDirection = { 0, 0, 1 };

	VelocityCurveOutput combinedOutput;
	CurveLib::Internal::CombineCurveOutput(combinedOutput, instance1);
	const auto& output1{ instance1.mOutput };
	const auto& output2{ instance2.mOutput };

	REQUIRE(combinedOutput.mDirection == output1.mDirection);
	REQUIRE(combinedOutput.mVelocity == output1.mVelocity);
	REQUIRE_THAT(combinedOutput.mSpeed, Catch::Matchers::WithinAbs(output1.mSpeed, sFloatEpsilon));
	REQUIRE(combinedOutput.mPosition == output1.mPosition);
	REQUIRE(combinedOutput.mPositionDelta == output1.mPositionDelta);
	REQUIRE(combinedOutput.mRotation == output1.mRotation);

	CurveLib::Internal::CombineCurveOutput(combinedOutput, instance2);
	REQUIRE(combinedOutput.mDirection == (output1.mDirection + output2.mDirection).GetNormalized());
	REQUIRE(combinedOutput.mVelocity == (output1.mVelocity + output2.mVelocity).GetNormalized());
	REQUIRE(combinedOutput.mSpeed == (output1.mVelocity + output2.mVelocity).GetNormalized().GetLength());
}

TEST_CASE("TickPlayback linear movement")
{
	using namespace CurveLib;

	constexpr float DISTANCE_TOLERANCE = 0.1f;

	VelocityCurveContext context;

	auto curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mSpeedMultiplier = 5.f;
	curveInstance.mMechanic.mPlayCount = CurveLib::PLAY_COUNT_INFINITE;

	StartVelocityCurve(context, curveInstance);
	REQUIRE(IsCurveRunning(context, curveInstance.mMechanic.mInstanceId));
	REQUIRE(context.mLinearCurves.size() == 1);
	REQUIRE(context.mRotationCurves.size() == 0);

	Float3 expectedPosition{};
	for (uint8_t i = 0; i < 100; ++i)
	{
		context.mAbsoluteTime = Seconds(i * TICK_DURATION.count());
		CurveLib::TickPlayback(context, context.mAbsoluteTime);

		REQUIRE(context.mOutput.mHasEverUpdated);
		
		// Need two data points for distance (area) to be calculated
		if (i > 0)
		{
			REQUIRE_THAT(context.mOutput.mSpeed, Catch::Matchers::WithinRel(curveInstance.mMechanic.mSpeedMultiplier));
			// Using GetLength() for test output readability
			REQUIRE_THAT(context.mOutput.mVelocity.GetLength(), Catch::Matchers::WithinRel(curveInstance.mMechanic.mSpeedMultiplier));

			// Ensure that we moved the expected distance within a certain margin of error
			// Since we're using a flat speed graph for this test, we can compute the expected position by dead reckoning
			expectedPosition = curveInstance.mMechanic.mStartPosition + curveInstance.mMechanic.mDirection * context.mAbsoluteTime.count() * curveInstance.mMechanic.mSpeedMultiplier;
			const auto positionError = expectedPosition - context.mOutput.mPosition;

			REQUIRE_THAT(positionError.GetLength(), Catch::Matchers::WithinAbs(0.f, DISTANCE_TOLERANCE));
		}
	}
	
	REQUIRE(context.mHasEverUpdated);
	
	// Ensure that conditions stay stable on curve stop
	CurveLib::StopVelocityCurve(context, curveInstance.mMechanic.mInstanceId);
	context.mAbsoluteTime += TICK_DURATION;
	CurveLib::TickPlayback(context, context.mAbsoluteTime);
	REQUIRE(context.mLinearCurves.empty());
	REQUIRE(context.mRotationCurves.empty());
	REQUIRE(context.mOutput.mVelocity.IsZero());
	REQUIRE(context.mOutput.mSpeed < sFloatEpsilon);
	const auto finalPositionError = expectedPosition - context.mOutput.mPosition;
	REQUIRE_THAT(finalPositionError.GetLength(), Catch::Matchers::WithinAbs(0.f, DISTANCE_TOLERANCE));
}

TEST_CASE("Infinite playback")
{
	using namespace CurveLib;

	VelocityCurveContext context;

	VelocityCurveInstance curveInstance = GenerateTestCurveInstance();
	curveInstance.mMechanic.mPlayCount = CurveLib::PLAY_COUNT_INFINITE;
	curveInstance.mMechanic.mRawAssetDuration = Seconds(0.5);
	curveInstance.mMechanic.mStretchDuration = Seconds(0.0); // No stretch (use asset duration)

	CurveLib::StartVelocityCurve(context, curveInstance);

	Seconds absoluteTime(0);
	for (int i = 0; i < 1000; ++i)
	{
		absoluteTime = Seconds(i * TICK_DURATION.count());
		CurveLib::TickPlayback(context, absoluteTime);
	}

	REQUIRE(CurveLib::IsCurveRunning(context, curveInstance.mMechanic.mInstanceId));
}