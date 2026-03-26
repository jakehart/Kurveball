#include "IntegratorFunctions.h"

namespace Kurveball
{
    template<typename ScalarType, typename VectorType>
    ScalarType IntegratorFunctions<ScalarType, VectorType>::LeftRiemann(const VectorType& previousSample, const VectorType& currentSample)
    {
        const ScalarType width{ currentSample.X - previousSample.X };
        return width * previousSample.Y;
    }

    template<typename ScalarType, typename VectorType>
    ScalarType IntegratorFunctions<ScalarType, VectorType>::RightRiemann(const VectorType& previousSample, const VectorType& currentSample)
    {
        const ScalarType width{ currentSample.X - previousSample.X };
        return width * currentSample.Y;
    }

    template<typename ScalarType, typename VectorType>
    ScalarType IntegratorFunctions<ScalarType, VectorType>::Trapezoidal(const VectorType& previousSample, const VectorType& currentSample)
    {
        const ScalarType width{ currentSample.X - previousSample.X };
        return width * (previousSample.Y + currentSample.Y) * 0.5f;
    }
}