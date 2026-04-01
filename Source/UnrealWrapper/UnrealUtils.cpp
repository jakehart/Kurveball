#include "UnrealWrapper/UnrealUtils.h"

#include "UnrealWrapper/KurveballLog.h"
#include "Vector3.h"

#include <Components/SplineComponent.h>

namespace Kurveball
{
    Float3 ToFloat3(const FVector& unrealVector)
    {
        return Kurveball::Float3(unrealVector.X, unrealVector.Y, unrealVector.Z);
    }

    FVector ToFVector(const Float3& curveLibVector)
    {
        return FVector(curveLibVector.X, curveLibVector.Y, curveLibVector.Z);
    }

    CurveSamplerXY CreateSamplerXY(const UCurveFloat* curveAsset)
    {
        static const CurveSamplerXY NULL_SAMPLER = [](float) {return 0.f; };
        
        if (!curveAsset)
        {
            UE_LOG(KurveballLog, Error, TEXT("CreateSamplerXY received null curveAsset"));
            return NULL_SAMPLER;
        }

        return [curveAsset](float curveX) -> float
            {
                // Since this lambda can be called later, ensure pointer is still valid
                if (curveAsset)
                {
                    return curveAsset->GetFloatValue(curveX);
                }

                UE_LOG(KurveballLog, Warning, TEXT("CreateSamplerXY sampling from curve that became null"));
                return 0.f;
            };
    }

    CurveSampler3D CreateUnrealSplineSampler(const USplineComponent* splineComponent, float desiredHeight)
    {
        static const CurveSampler3D NULL_SAMPLER = [](float) {return Kurveball::Float3(0, 0, 0); };

        if (!splineComponent || splineComponent->GetSplineLength() < sFloatEpsilon)
        {
            UE_LOG(KurveballLog, Error, TEXT("CreateUnrealSplineSampler received null or empty spline"));
            return NULL_SAMPLER;
        }

        // Scale to desiredHeight
        float heightScale = 1.f;
        if (desiredHeight > sFloatEpsilon)
        {
            const auto localBounds = splineComponent->CalcLocalBounds();
            const float nativeSplineHeight = localBounds.GetBox().Max.Z - localBounds.GetBox().Min.Z;
            if (nativeSplineHeight > sFloatEpsilon)
            {
                heightScale = desiredHeight / nativeSplineHeight;
            }
        }

        return [splineComponent, heightScale](float distance)
            {
                // Make sure the spline is still valid at the time of sampling
                if (splineComponent && splineComponent->IsValidLowLevelFast(false))
                {
                    // Return the position at this arc distance
                    const FVector rawPosition = splineComponent->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);
                    return Kurveball::Float3(rawPosition.X, rawPosition.Y, rawPosition.Z * heightScale);
                }

                UE_LOG(KurveballLog, Warning, TEXT("CreateUnrealSplineSampler sampling from spline that became null"));
                return Kurveball::Float3(0, 0, 0);
            };
    }
}