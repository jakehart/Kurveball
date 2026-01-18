// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <cstdint>
#include <chrono>

#include "AreaAccumulator.h"
#include "CircularBuffer.h"
#include "CurveSampler.h"
#include "CurveSampler3D.h"
#include "CurveMechanic.h"
#include "VelocityCurveOutput.h"

namespace CurveLib
{
	constexpr size_t NUM_HISTORY_ENTRIES = 256U;

	struct VelocityCurveInstance
	{
		// The curve's description as received from the user and sanitized in StartVelocityCurve.
		CurveMechanic mMechanic {};
		
		// Each tick, TickPlayback (in VelocityCurvePlayback.h) samples the speed from the velocity curve,
		// then feeds it into this accumulator that calculates the area under the curve at each update step
		// (which is used to calculate position delta), and adds it to the total area (which corresponds to
		// total distance travelled and is used to calculate the current position).
		AreaAccumulator mDistanceAccumulator {};
		
		// Given a time (x), returns a speed value (y).
		CurveSamplerXY mSpeedSampler;
		
		// Given some distance travelled, returns a position. This exists so that we can use a velocity curve
		// to move along a 3D world-space spline.
		std::optional<CurveSampler3D> mPositionSampler = std::nullopt;
		
		// The output (position, rotation, speed, etc.) of this individual curve from the most recent tick.
		// This is combined with any other running curves to give the final result.
		CurveLib::VelocityCurveOutput mOutput {};
		
		// Saves a history of this curve's most recent output velocities (with speedMultiplier multiplied in)
		// for debug purposes.
		// TODO: Make debug information completely optional to save memory, and make its size adjustable by the user
		CircularBuffer<Float2, NUM_HISTORY_ENTRIES> mHistory {};

		std::string ToString() const
		{
			std::stringstream ss;

			const std::string debugName = mMechanic.mDebugName ? *mMechanic.mDebugName : "Unnamed";
			ss << debugName << ": "
				<< mMechanic.mSpeedMultiplier << " m/s, dir "
				<< mMechanic.mDirection.ToString()
				<< ", stretchLen: " << mMechanic.mStretchDuration;

			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const VelocityCurveInstance& instance)
	{
		// Call object's string method
		os << instance.ToString();

		// Return stream
		return os;
	}
}