// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "Kurveball/Vector3.h"
#include "Kurveball/CurveMechanic.h"
#include "Kurveball/UnrealWrapper/ECoordinateSpace.h"
#include "Kurveball/UnrealWrapper/EAxisMode.h"
#include "Kurveball/UnrealWrapper/UnrealUtils.h"

#include "UCurveMechanic.generated.h"

// Wraps Kurveball::CurveMechanic in a way that Unreal can understand, adding
// the ability for users to create them as Unreal DataAssets or in DataTables right there
// in their Content pane. This makes it easier to modularly load and unload movement
// mechanics.
UCLASS(BlueprintType)
class CURVEDEMO_API UCurveMechanic : public UDataAsset, public FTableRowBase
{
    GENERATED_BODY()
    
public:
    // An Unreal CurveFloat asset that you use to control the speed of the actor. The x axis
    // is time, and y is speed.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCurveFloat* VelocityCurveAsset;
    // Must be unique. A name for this movement mechanic.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CurveInstanceName;
    // The direction that the velocity curve should go. Can be in local or world space, depending on
    // the value of CoordinateSpace below. This isn't relevant for rotation mechanics or for spline-following
    // mechanics.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Direction;
    // This number gets multiplied with your VelocityCurveAsset. If you defined your curve to have a maximum
    // height of y=1, SpeedMultiplier is in world units per second (centimeters per second). Negative speeds
    // (here or in your VelocityCurveAsset) go backwards.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpeedMultiplier;
    // This option defines whether the velocity curve causes translation or rotation,
    // and it decides which axes the mechanic should act upon. Axes can be in local or world
    // space, depending on the CoordinateSpace option below.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAxisMode AxisMode = EAxisMode::allMovementAxes;
    // If true, teleport the actor to CustomStartPosition before beginning the mechanic.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool UseCustomStartPosition;
    // The position to teleport to when the mechanic starts (if UseCustomStartPosition is enabled).
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector CustomStartPosition;
    // If nonzero, VelocityCurveAsset will be stretched to this time in seconds. If zero,
    // the curve will be unstretched, its x axis interpreted directly in seconds.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StretchDuration = 0;
    // The number of times to play the looped portion of the curve. If zero, loop forever.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 PlayCount = 0;
    // The beginning of the part of VelocityCurveAsset you want to loop. If both LoopStartX and
    // LoopEndX are zero, the entire curve will be looped.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LoopStartX = 0;
    // The end of the part of VelocityCurveAsset you want to loop. If both LoopStartX and
    // LoopEndX are zero, the entire curve will be looped.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LoopEndX = 0;
    // The mechanic can run in either local or world space, which affects the Direction, AxisMode, and
    // CustomStartPosition options.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECoordinateSpace CoordinateSpace = ECoordinateSpace::world;

    Kurveball::CurveInstanceId GetCurveId() const
    {
        return GetTypeHash(CurveInstanceName);
    }

    Kurveball::CurveMechanic ToNative() const
    {
        return
        {
            .mInstanceId = GetTypeHash(CurveInstanceName),
            .mDebugName = std::string(TCHAR_TO_UTF8(*CurveInstanceName.ToString())),
            .mDirection = Kurveball::ToFloat3(Direction),
            .mCoordinateSpace = static_cast<Kurveball::CoordinateSpace>(CoordinateSpace),
            .mSpeedMultiplier = SpeedMultiplier,
            .mAxisMode = static_cast<Kurveball::AxisMode>(AxisMode),
            .mStartTime = Kurveball::Seconds(0),
            .mStartPosition = UseCustomStartPosition ? Kurveball::ToFloat3(CustomStartPosition) : Kurveball::Float3(),
            .mStretchDuration = Kurveball::Seconds(StretchDuration),
            .mRawAssetDuration = Kurveball::Seconds(VelocityCurveAsset ? VelocityCurveAsset->FloatCurve.GetLastKey().Time : 1.f),
            .mPlayCount = (uint32)PlayCount,
            .mLoopStartX = LoopStartX,
            .mLoopEndX = LoopEndX
        };
    }
};
