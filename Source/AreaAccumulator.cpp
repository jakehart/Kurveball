// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#include "AreaAccumulator.h"
#include "IntegratorFunctions.h"
#include "Vector2.h"

namespace CurveLib
{
    AreaAccumulator::AreaAccumulator()
        : mIntegrator(IntegratorFunctions<float, Float2>::Trapezoidal)
    {
    }

    AreaAccumulator::AreaAccumulator(IntegratorFunction integrator)
        : mIntegrator(integrator)
    {
    }

    void AreaAccumulator::Reset()
    {
        mTotalArea = 0.f;
        mLatestSample.X = 0.f;
        mLatestSample.Y = 0.f;
        mHasUpdated = false;
    }

    bool AreaAccumulator::HasEverUpdated() const
    {
        return mHasUpdated;
    }

    void AreaAccumulator::AccumulateArea(float x, float y)
    {
        AccumulateArea(Float2(x, y));
    }
    
    void AreaAccumulator::AccumulateArea(const Float2& sample)
    {
        if (mHasUpdated)
        {
            mLatestStepArea = mIntegrator(mLatestSample, sample);
            mTotalArea += mLatestStepArea;
        }

        mLatestSample = sample;
        mHasUpdated = true;
    }

    float AreaAccumulator::GetTotalArea() const
    {
        return mTotalArea;
    }
    
    float AreaAccumulator::GetLatestStepArea() const
    {
        return mLatestStepArea;
    }

    const Float2& AreaAccumulator::GetLatestSample() const
    {
        return mLatestSample;
    }
}