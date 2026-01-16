// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "CurveLib/Vector3.h"
#include "CurveLib/CurveMechanic.h"
#include "CurveLib/UnrealWrapper/ECoordinateSpace.h"
#include "CurveLib/UnrealWrapper/EAxisMode.h"
#include "CurveLib/UnrealWrapper/UnrealUtils.h"

#include "UCurveMechanic.generated.h"

// Wraps CurveLib::VelocityCurveDescription in a way that Unreal can understand, adding
// the ability for users to create them as Unreal DataAssets or in DataTables right there
// in their Content pane. This makes it easier to modularly load and unload movement
// mechanics.
UCLASS(BlueprintType)
class CURVEDEMO_API UCurveMechanic : public UDataAsset, public FTableRowBase
{
	GENERATED_BODY()
	
public:
	// TODO: Is there a safer yet still Unreal-approved way to do this without needing to use a raw pointer?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* VelocityCurveAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurveInstanceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAxisMode AxisMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double StartTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseCustomStartPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CustomStartPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StretchDuration;
	// Sadly has to be int64, since Unreal properties can't be uint32 like the underlying API
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 PlayCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoopStartX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoopEndX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECoordinateSpace CoordinateSpace = ECoordinateSpace::world;

	CurveLib::CurveInstanceId GetCurveId() const
	{
		return GetTypeHash(CurveInstanceName);
	}

	CurveLib::CurveMechanic ToNative() const
	{
		return
		{
			.mInstanceId = GetTypeHash(CurveInstanceName),
			.mDebugName = std::string(TCHAR_TO_UTF8(*CurveInstanceName.ToString())),
			.mDirection = CurveLib::ToFloat3(Direction),
			.mCoordinateSpace = static_cast<CurveLib::CoordinateSpace>(CoordinateSpace),
			.mSpeedMultiplier = SpeedMultiplier,
			.mAxisMode = static_cast<CurveLib::AxisMode>(AxisMode),
			.mStartTime = CurveLib::Seconds(StartTime),
			.mStartPosition = UseCustomStartPosition ? CurveLib::ToFloat3(CustomStartPosition) : CurveLib::Float3(),
			.mStretchDuration = CurveLib::Seconds(StretchDuration),
			.mRawAssetDuration = CurveLib::Seconds(VelocityCurveAsset ? VelocityCurveAsset->FloatCurve.GetLastKey().Time : 1.f),
			.mPlayCount = (uint32)PlayCount,
			.mLoopStartX = LoopStartX,
			.mLoopEndX = LoopEndX
		};
	}
};
