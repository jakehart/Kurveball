#pragma once

#include "ContainerTypes.h"

namespace Kurveball
{
    template<typename ScalarT>
    class LookupTable
    {
    public:
        using ScalarType = ScalarT;

        LookupTable(uint32_t length);

        void AddSample(ScalarType x, ScalarType y);

        // Linearly interpolates between samples to find the nearest y for a given x
        ScalarType CalculateValue(ScalarType x);

        ScalarT GetMinX() const;
        ScalarT GetMaxX() const;

        size_t GetSampleRate() const;

    private:
        size_t mSampleRate{ 64 };

        // (x, y)
        using Sample = Pair<ScalarType, ScalarType>;

        // Contains the samples, presumed to be added in order by the calling code
        Vector<Sample> mSamples;
    };
}

#include "LookupTable.inl"