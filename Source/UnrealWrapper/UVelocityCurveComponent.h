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

    // Starts a curve-driven movement mechanic on this actor. The x axis is time, and the y axis
    // is speed, with many customizable parameters to loop, stretch, and otherwise control the curve.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void StartVelocityCurve(const UCurveMechanic* curve);
    // Stops a specific curve mechanic that is running on this actor.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void StopVelocityCurve(const UCurveMechanic* mechanic);
    // Seeks to the loop endpoint of the curve (as defined by mLoopEnd in UCurveMechanic) and lets
    // the curve play out to the end. In other words, it plays the "outro" of the curve, which is
    // usually the deceleration portion.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void SoftStopVelocityCurve(const UCurveMechanic* mechanic);
    // Stops all curve mechanics that this actor is running.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void StopAllVelocityCurves();

    // Seeks the velocity curve playhead to a specific coordinate on the curve asset.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void SeekToX(const UCurveMechanic* mechanic, float curveXCoordinate);

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

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    float GetMechanicSpeed(const UCurveMechanic* mechanic) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    float GetTotalSpeed() const;

    // Returns true if the actor is running the specific mechanic given.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    bool IsCurveRunning(const UCurveMechanic* mechanic) const;
    // Returns true if this actor is running any velocity curve.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    bool IsAnyCurveRunning(bool includeLinear = true, bool includeRotational = true) const;

    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void AttachSpline(const UCurveMechanic* mechanic, const USplineComponent* splineComponent, float desiredHeight = 0.f);

    // Stretches a spline in place so that its start and end are at the desired points, with the top of the spline
    // at the given height in cm. The input coordinates are in terms of whatever coordinate space you specify.
    /*UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    const void StretchSpline(USplineComponent* splineComponent, FVector startPosition, FVector endPosition, float height, ECoordinateSpace coordinateSpace = ECoordinateSpace::world);*/

    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void SetLocation(FVector location);

    // Returns the actor's current velocity, as given by the combined output of the velocity curves that are running.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    FVector GetVelocity();

    // Returns the actor's velocity in local space.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    FVector GetLocalVelocity();

    // Returns the angular velocity around each axis in degrees per second, which is output from the rotation curves.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    FVector GetAngularVelocity();

    // Shows a simple on-screen graph listing each curve and its speed over time for debugging purposes.
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void ShowCurveDebugger(bool show = true);
    // Returns an individual mechanic's output from this frame as a debug string for logging.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    FString GetCurveOutputAsDebugString(const UCurveMechanic* mechanic) const;
    // Returns the total velocity curve output from this frame as a debug string for logging.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    FString GetFinalOutputAsDebugString() const;
    // Returns a list of all of the velocity curves that are running on this actor for debugging.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    FString GetRunningCurvesAsDebugString() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VelocityCurves")
    int32 CurveNameToInstanceId(const FString& curveName) const;

    DECLARE_DYNAMIC_DELEGATE_RetVal(float, FCurveXSampler);
    UFUNCTION(BlueprintCallable, Category = "VelocityCurves")
    void DefineCurveXFunction(const UCurveMechanic* mechanic, const FCurveXSampler& xSampler);

    const CurveLib::VelocityCurveContext& GetCurveContext() const;

    // If true, collision stops this actor. If false, the actor can clip through collision.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool RespectCollision = true;

    // If true, control the actor using velocity and leave the physics simulation up to Unreal.
    // If false, control the actor using position and ignore Unreal physics.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool OutputVelocity = true;

private:
    CurveLib::VelocityCurveContext& AccessCurveContext();

    // Helper function to return the camera's rotation if isCameraRelative == true, or the character's
    // rotation otherwise.
    FRotator GetCameraRotation();

    void SendVelocityToUnreal(const CurveLib::Float3& velocity, const CurveLib::Float3& angularVelocity);

    CurveLib::VelocityCurveContext mCurveContext;
    UVelocityCurveDebugger* mCurveDebugger = nullptr;
};