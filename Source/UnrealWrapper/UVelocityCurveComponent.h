// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "CurveLib/VelocityCurveContext.h"
#include "CurveLib/VelocityCurveInstance.h"
#include "CurveLib/UnrealWrapper/UCurveMechanic.h"

#include "UVelocityCurveComponent.generated.h"

class USplineComponent;
class USplineMeshComponent;
class UVelocityCurveDebugger;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CURVEDEMO_API UVelocityCurveComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
public:    
    UVelocityCurveComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Blueprint-exposed wrappers for VelocityCurveContext functions, using mCurveContext.
    // Blueprint only supports int32 and uint8.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void StartVelocityCurve(const UCurveMechanic* curve);
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void UpdateVelocityCurve(const UCurveMechanic* mechanic, bool updateSpeed, float speedMultiplier, bool updateDirection, FVector direction);
    // Sends an input axis into two velocity curves, using camera-relative or character-relative ("tank") controls depending on isCameraRelative.
    // forwardMechanic and sideMechanic can be anything, but forwardSpace is usually a local-space forward-facing mechanic, and sideMechanic is
    // usually a yaw or local-space sideways-facing mechanic.
    // This function automatically starts or stops the velocity curves as needed.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void InputToVelocityCurves(const UCurveMechanic* forwardMechanic, const UCurveMechanic* sideMechanic, const FVector2D& inputAxes, bool isCameraRelative = true);
    // Sends a single input axis into a single velocity curve, starting or stopping the curve as needed.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void InputAxisToVelocityCurve(const UCurveMechanic* mechanic, float inputAxis);
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void StopVelocityCurve(const UCurveMechanic* mechanic);
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    bool IsCurveRunning(const UCurveMechanic* mechanic) const;

    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void AttachSpline(const UCurveMechanic* mechanic, const USplineComponent* splineComponent);

    // Stretches a spline in place so that its start and end are at the desired points, with the top of the spline
    // at the given height in cm. The input coordinates are in terms of whatever coordinate space you specify.
    /*UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    const void StretchSpline(USplineComponent* splineComponent, FVector startPosition, FVector endPosition, float height, ECoordinateSpace coordinateSpace = ECoordinateSpace::world);*/

    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void SetLocation(FVector location);

    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void ShowCurveDebugger(bool show = true);
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    FString GetCurveOutputAsDebugString(const UCurveMechanic* mechanic) const;
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    FString GetFinalOutputAsDebugString() const;
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    FString GetRunningCurvesAsDebugString() const;

    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    int32 CurveNameToInstanceId(const FString& curveName) const;

    CurveLib::VelocityCurveContext& AccessCurveContext();
    const CurveLib::VelocityCurveContext& GetCurveContext() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool RespectCollision = true;

    // If true, control the actor using velocity and leave the physics simulation up to Unreal.
    // If false, control the actor using position and ignore Unreal physics.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool OutputVelocity = true;

private:
    // Helper function to return the camera's rotation if isCameraRelative == true, or the character's
    // rotation otherwise.
    FRotator GetRotationToApply(bool isCameraRelative);

    void SendVelocityToUnreal(const CurveLib::Float3& velocity, const CurveLib::Float3& angularVelocity);

    CurveLib::VelocityCurveContext mCurveContext;
    UVelocityCurveDebugger* mCurveDebugger = nullptr;
};