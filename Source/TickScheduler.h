#pragma once
#include <functional>
#include "UnitTypes.h"

namespace Kurveball
{
	// Receives variable timestep ticks via VariableTick() and pieces them out to subticks that are a fixed timestep apart.
	// On each call of VariableTick(), we run all the fixed ticks within it that we can, calling mFullSubtickCallback for each
	// whole subtick and mPartialSubtickCallback for the leftover time that remains.
	class TickScheduler
	{
	public:
		TickScheduler() = default;
		TickScheduler(Seconds fixedSubtickDuration);
	
		// Each time a fixed subtick happens, we call a callback with this signature. The first parameter is the current
		// absoluteTime, and the second is the fixed subtick duration.
		using FixedSubtickCallback = std::function<void(Seconds, Seconds)>;
		
		// Called to notify the user of the leftover time that remains after all possible fixed ticks have occurred.
		// The first parameter is absoluteTime, and the second parameter is the duration past between the end of the last fixed
		// subtick and now.
		using PartialSubtickCallback = std::function<void(Seconds, Seconds)>;
		
		// Call this from your variable tick function. It will schedule fixed subticks automatically and call the fixed tick
		// callbacks defined below.
		void VariableTick(Seconds absoluteTime);

		void SetFullSubtickCallback(FixedSubtickCallback subtickCallback);
		void SetPartialSubtickCallback(PartialSubtickCallback partialCallback);

	private:
		// The last absoluteTime at which this scheduler was called. This is updated when VariableTick() is called.
		Seconds mLastVariableTickTime{ 0.f };
		
		// The guaranteed time between each subtick.
		Seconds mFixedSubtickDuration{ 0.05f };
		
		// The function that gets called during each fixed subtick.
		FixedSubtickCallback mFullSubtickCallback;
		
		// When there's time left over between the current absoluteTime, but not enough to do a full additional subtick,
		// we call this function. Note that the mFullSubtickCallback will still be called when enough time has accumulated,
		// so if you change state in this callback, you'll want to keep it separate so you can discard it.
		PartialSubtickCallback mPartialSubtickCallback;
	};
}

#include "TickScheduler.inl"