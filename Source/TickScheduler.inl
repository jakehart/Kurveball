#pragma once
#include <cstdint>
#include <cmath>

#include "Asserts.h"
#include "MathUtils.h"

namespace Kurveball
{
	inline TickScheduler::TickScheduler(Seconds fixedSubtickDuration)
		 : mFixedSubtickDuration(fixedSubtickDuration)
	{
	}

	inline void TickScheduler::VariableTick(Seconds absoluteTime)
	{
		Seconds timeSinceLastCall = absoluteTime - mLastVariableTickTime;
		const float numFullSubticksFloat = ThresholdToZero(std::floor(timeSinceLastCall / mFixedSubtickDuration));

		KURVEBALL_VERIFY_RETURN(numFullSubticksFloat < std::numeric_limits<uint16_t>::max());

		uint16_t numFullSubticks = static_cast<uint16_t>(numFullSubticksFloat);

		for (uint16_t i = 0; i < numFullSubticks; ++i)
		{
			mFullSubtickCallback(mLastVariableTickTime + i * mFixedSubtickDuration, mFixedSubtickDuration);
		}

		const Seconds remainingTime = Seconds(ThresholdToZero((absoluteTime - timeSinceLastCall).count()));
		mPartialSubtickCallback(remainingTime, absoluteTime);

		mLastVariableTickTime = absoluteTime;
	}

	inline void TickScheduler::SetFullSubtickCallback(FixedSubtickCallback subtickCallback)
	{
		mFullSubtickCallback = subtickCallback;
	}

	inline void TickScheduler::SetPartialSubtickCallback(PartialSubtickCallback partialCallback)
	{
		mPartialSubtickCallback = partialCallback;
	}
}