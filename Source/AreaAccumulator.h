// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "Vector2.h"

#include "IntegratorFunctionType.h"

namespace CurveLib
{
	// Collects incoming samples, applies a calculus integration function to them, and
	// progressively accumulates the area under the curve.
	class AreaAccumulator
	{
	public:
		AreaAccumulator();
		AreaAccumulator(IntegratorFunction integrator);
		void Reset();
		bool HasEverUpdated() const;

		void AccumulateArea(float x, float y);
		void AccumulateArea(const Float2& sample);
		float GetTotalArea() const;
		float GetLatestStepArea() const;
		const Float2& GetLatestSample() const;

	private:
		// The total accumulated area from all integrator steps that have run.
		float mTotalArea{ 0.f };
		// The area from ONLY the most recent integration step.
		float mLatestStepArea{ 0.f };
		bool mHasEverUpdated{ false };
		IntegratorFunction mIntegrator;
		Float2 mLatestSample{};
	};
}