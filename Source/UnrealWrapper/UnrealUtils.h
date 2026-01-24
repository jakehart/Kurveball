#pragma once

#include "CurveLib/CurveSampler.h"
#include "CurveLib/CurveSampler3D.h"

class UCurveFloat;
class USplineComponent;

namespace CurveLib
{
    template<typename ScalarType>
    class Vector3;
    using Float3 = Vector3<float>;

    Float3 ToFloat3(const FVector& unrealVector);
    FVector ToFVector(const Float3& curveLibVector);
    
    CurveSamplerXY CreateSamplerXY(const UCurveFloat* curveAsset);
    CurveSampler3D CreateUnrealSplineSampler(const USplineComponent* splineComponent, float desiredHeight = 0.f);
}