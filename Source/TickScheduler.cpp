#pragma once
#include "TickScheduler.h"
#include <cstdint>
#include <cmath>

#include "Asserts.h"
#include "MathUtils.h"

namespace Kurveball
{
	TickScheduler::TickScheduler(Seconds fixedSubtickDuration)
		: mFixedSubtickDuration(fixedSubtickDuration)
	{
	}

	void TickScheduler::VariableTick(Seconds absoluteTime)
	{
		Seconds timeSinceLastCall = absoluteTime - mLastCompletedSubtickTime;
		const float numFullSubticksFloat = ThresholdToZero(std::floor(timeSinceLastCall / mFixedSubtickDuration));

		KURVEBALL_VERIFY_RETURN(numFullSubticksFloat < std::numeric_limits<uint16_t>::max());

		const uint16_t numFullSubticks = static_cast<uint16_t>(numFullSubticksFloat);
		if (numFullSubticks == 0)
		{
			mPartialSubtickCallback(absoluteTime, timeSinceLastCall);
			return;
		}

		for (uint16_t i = 0; i < numFullSubticks; ++i)
		{
			mFullSubtickCallback(mLastCompletedSubtickTime + i * mFixedSubtickDuration, mFixedSubtickDuration);
		}

		const Seconds endOfLastFullSubtick = mLastCompletedSubtickTime + numFullSubticks * mFixedSubtickDuration;
		const Seconds remainingTime = Seconds(ThresholdToZero((absoluteTime - endOfLastFullSubtick).count()));

		if (remainingTime.count() > sFloatEpsilon)
		{
			mPartialSubtickCallback(absoluteTime, remainingTime);
		}

		mLastCompletedSubtickTime = endOfLastFullSubtick;
	}

	void TickScheduler::SetFixedSubtickDuration(Seconds fixedSubtickDuration)
	{
		mFixedSubtickDuration = fixedSubtickDuration;
	}

	void TickScheduler::SetFullSubtickCallback(FixedSubtickCallback subtickCallback)
	{
		mFullSubtickCallback = subtickCallback;
	}

	void TickScheduler::SetPartialSubtickCallback(PartialSubtickCallback partialCallback)
	{
		mPartialSubtickCallback = partialCallback;
	}
}