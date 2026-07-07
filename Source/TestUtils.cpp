#include "TestUtils.h"
#include "VelocityCurveApi.h"
#include "VelocityCurveContext.h"
#include "VelocityCurvePlayback.h"

namespace Kurveball
{
    Kurveball::VelocityCurveInstance GenerateTestCurveInstance()
    {
        using namespace Kurveball;

        const auto defaultSamplerFunction = []([[maybe_unused]] float curveX) -> float
            {
                return 1.f;
            };

        VelocityCurveInstance testCurve
        {
            .mMechanic =
            {
                .mInstanceID = std::rand() % std::numeric_limits<CurveInstanceID>::max(),
                .mDebugName = "Test Curve",
                .mDirection = {1, 0, 0},
                .mCoordinateSpace = CoordinateSpace::world,
                .mSpeedMultiplier = 123.f,
                .mAxisMode = AxisMode::allMovementAxes,
                .mStartTime = Seconds(0),
                .mStretchDuration = Seconds(1),
                .mRawAssetDuration = Seconds(1),
                .mPlayCount = PLAY_COUNT_INFINITE,
                .mLoopStartX = 0,
                .mLoopEndX = 0
            },

            .mDistanceAccumulator = {},
            .mSpeedSampler = defaultSamplerFunction,
            .mOutput = {}
        };

        return testCurve;
    }

    void TickCurveContext(Kurveball::VelocityCurveContext& ioContext, Kurveball::Seconds tickDuration, size_t numTicks)
    {
        using namespace Kurveball;

        // Clear previous state to assure a clean slate for testing
        ioContext.mAbsoluteTime = {};

        for (size_t i = 0; i < numTicks; ++i)
        {
            const Seconds absoluteTime = tickDuration * i;
            TickPlayback(ioContext, absoluteTime);
        }
    }

    Kurveball::Seconds RunCurveAndGetEndTime(Kurveball::VelocityCurveContext& ioContext, Kurveball::VelocityCurveInstance& ioCurveInstance, Seconds tickDuration, size_t maxNumTicks)
    {
        using namespace Kurveball;

        // Start the timer at zero so that successive curves in the same testcase don't depend on each other
        ioContext.mAbsoluteTime = {};

        StartVelocityCurve(ioContext, ioCurveInstance);

        // Safeguard against infinite execution
        for (uint16_t i = 0; i < maxNumTicks; ++i)
        {
            // Use the short tick time so we can verify end time with more precision
            TickPlayback(ioContext, Seconds(i * tickDuration));

            if (!IsCurveRunning(ioContext, ioCurveInstance.mMechanic.mInstanceID))
            {
                break;
            }
        }

        return ioContext.mAbsoluteTime;
    }
}