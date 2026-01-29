#pragma once

#include "MathUtils.h"

namespace CurveLib
{
    template<typename PositionT>
    LookupTable<PositionT>::LookupTable(uint32_t length)
        : mSampleRate(length)
    {
    }

    template<typename PositionT>
    void LookupTable<PositionT>::AddSample(ScalarType x, ScalarType y)
    {
        mSamples.emplace_back(x, y);
    }

    template<typename PositionT>
    LookupTable<PositionT>::ScalarType LookupTable<PositionT>::CalculateValue(ScalarType x)
    {
        if (x < 0)
        {
            return 0;
        }

        const size_t lowerIndex = (size_t)(x * mSampleRate);
        const size_t higherIndex = lowerIndex + 1;
        
        // Ensure that the table covers the points that we need
        if (higherIndex >= mSamples.size())
        {
            return 0;
        }

        const ScalarType lowerSampleY = mSamples[lowerIndex].second;
        const ScalarType higherSampleY = mSamples[higherIndex].second;
        const ScalarType sampleWidth = mSamples[higherIndex].first - mSamples[lowerIndex].first;

        const ScalarType distanceBetweenSamples = 1.f / mSampleRate;
        const ScalarType progressBetweenSamples = (x - mSamples[lowerIndex].first) / sampleWidth;

        return Lerp(lowerSampleY, higherSampleY, progressBetweenSamples);
    }

    template<typename PositionT>
    LookupTable<PositionT>::ScalarType LookupTable<PositionT>::GetMinX() const
    {
        return mSamples.front().first;
    }

    template<typename PositionT>
    LookupTable<PositionT>::ScalarType LookupTable<PositionT>::GetMaxX() const
    {
        return mSamples.back().first;
    }

    template<typename PositionT>
    size_t LookupTable<PositionT>::GetSampleRate() const
    {
        return mSampleRate;
    }
}