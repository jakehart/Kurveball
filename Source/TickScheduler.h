#pragma once
#include <functional>

namespace Kurveball
{
	// Receives variable timestep ticks via VariableTick() and pieces them out to subticks that are a fixed timestep apart.
	// On each call of VariableTick(), we run all the fixed ticks within it that we can, calling mFixedSubtickCallback for each
	// whole subtick and mPartialSubtickCallback for the leftover bit of time that remains.
	class TickScheduler
	{
	public:
		TickScheduler() = default;
		TickScheduler(float fixedSubtickDuration);
	
		// Each time a fixed subtick happens, we call a callback with this signature. The first float is the current
		// absoluteTime, and the second float is the fixed subtick duration.
		using FixedSubtickCallback = std::function<void(float, float)>;
		
		// Same as FixedSubtickCallback, except that the second float is the duration past between the end of the last fixed
		// subtick and now.
		using PartialSubtickCallback = std::function<void(float, float)>;
		
		// Call this from your variable tick function. It will schedule fixed subticks automatically and call the fixed tick
		// callbacks defined below.
		void VariableTick(float absoluteTime);
	private:
		// The last absoluteTime at which this scheduler was called. This is updated when VariableTick() is called.
		float mLastVariableTickTime = 0.f;
		
		float mFixedSubtickDuration = 0.05f;
		
		// The function that gets called during each fixed subtick.
		FixedSubtickCallback mFixedSubtickCallback;
		
		// When there's time left over between the current absoluteTime, but not enough to do a full additional subtick,
		// we call this function. Note that the mFixedSubtickCallback will still be called when enough time has accumulated,
		// so if you change state in this callback, you'll want to keep it separate so you can discard it.
		PartialSubtickCallback mPartialSubtickCallback;
	};
}

#include "TickScheduler.inl"