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
		// Each time a fixed subtick happens, we call a callback with this signature. The first parameter is the current
		// absoluteTime, and the second is the fixed subtick duration.
		using FixedSubtickCallback = std::function<void(Seconds, Seconds)>;

		// Called to notify the user of the leftover time that remains after all possible fixed ticks have occurred.
		// The first parameter is absoluteTime, and the second parameter is the duration past between the end of the last fixed
		// subtick and now.
		using PartialSubtickCallback = std::function<void(Seconds, Seconds)>;

		TickScheduler() = default;
		TickScheduler(Seconds fixedSubtickDuration, FixedSubtickCallback fullSubtickCallback, PartialSubtickCallback partialCallback);

		
		// Call this from your variable tick function. It will schedule fixed subticks automatically and call the fixed tick
		// callbacks defined below.
		void VariableTick(Seconds absoluteTime);

		// Sets the time between each virtual subtick. This determines how finely the subticks are scheduled.
		void SetFixedSubtickDuration(Seconds fixedSubtickDuration);

		// See mFullSubtickCallback below.
		void SetFullSubtickCallback(FixedSubtickCallback subtickCallback);

		// See mPartialSubtickCallback below.
		void SetPartialSubtickCallback(PartialSubtickCallback partialCallback);

		static constexpr Seconds sDefaultSubtickDuration{ 0.05f };

	private:
		// The timestamp of the last full subtick that was completed, in terms of absoluteTime. This is updated when VariableTick() is called.
		Seconds mLastCompletedSubtickTime{ 0.f };
		
		// The guaranteed time between each subtick.
		Seconds mFixedSubtickDuration{ sDefaultSubtickDuration };
		
		// The function that gets called during each fixed subtick.
		FixedSubtickCallback mFullSubtickCallback;
		
		// When there's time left over between the current absoluteTime, but not enough to do a full additional subtick,
		// we call this function. Note that the mFullSubtickCallback will still be called when enough time has accumulated,
		// so if you change state in this callback, you'll want to keep it separate so you can discard it.
		PartialSubtickCallback mPartialSubtickCallback;
	};
}
