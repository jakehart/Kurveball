// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#if defined(__UNREAL__)

#include "CurveLib/UnrealWrapper/UVelocityCurveComponent.h"

#include <Blueprint/UserWidget.h>
#include <Engine/Engine.h>
#include <GameFramework/PlayerController.h>
#include <Kismet/GameplayStatics.h>

#include <sstream>

#include "CurveLibLog.h"
#include "CurveLib/ContainerUtils.h"
#include "CurveLib/MathUtils.h"
#include "CurveLib/UnrealWrapper/USensorComponent.h"
#include "CurveLib/UnitTypes.h"
#include "CurveLib/VelocityCurveApi.h"
#include "CurveLib/VelocityCurveInstance.h"
#include "CurveLib/VelocityCurvePlayback.h"

#include "UVelocityCurveDebugger.h"

// Sets default values for this component's properties
UVelocityCurveComponent::UVelocityCurveComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UVelocityCurveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    const auto* world = GetWorld();
    if (!world)
    {
        return;
    }

    const auto absoluteTime = world->TimeSeconds;

    // Tell CurveLib about any position change that happened due to collision or simulated physics
    const FVector position = owner->GetActorLocation();
    CurveLib::SetPosition(mCurveContext, position.X, position.Y, position.Z);
    
    CurveLib::TickPlayback(mCurveContext, CurveLib::Seconds(absoluteTime));

    // The velocity curve context doesn't care which world units we use as long as we're consistent.
    // Here we're using Unreal units (cm) directly.
    const FVector newPosition = CurveLib::ToFVector(mCurveContext.mOutput.mPosition);
    // Rotation always uses degrees
    const FRotator newRotation = FRotator(mCurveContext.mOutput.mRotation.X, mCurveContext.mOutput.mRotation.Z, mCurveContext.mOutput.mRotation.Y);
    
    /*if (OutputVelocity)
    {
        SendVelocityToUnreal(mCurveContext.mOutput.mVelocity, mCurveContext.mOutput.mAngularVelocity);
    }
    else*/
    {
        owner->TeleportTo(newPosition, newRotation, false, !RespectCollision);
    }
}

void UVelocityCurveComponent::BeginPlay()
{
    Super::BeginPlay();

    auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    // Unreal uses Z+ up. CurveLib needs to know this so that it can mask off the
    // correct axes when you use eAxisMode::vertical or eAxisMode::horizontal.
    CurveLib::SetVerticalAxis(mCurveContext, Axis::Z);

    const FRotator rotation = owner->GetActorRotation();
    const FVector euler = rotation.Euler();
    CurveLib::SetRotation(mCurveContext, euler.X, euler.Y, euler.Z);

    // Position is updated in TickComponent
}

void UVelocityCurveComponent::StartVelocityCurve(const UCurveMechanic* mechanic)
{
    if (!mechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("ERROR: StartVelocityCurve (in Blueprint) needs to have a Movement Mechanic Asset specified!"));
        return;
    }

    if (!mechanic->VelocityCurveAsset)
    {
        UE_LOG(CurveLibLog, Error, TEXT("ERROR: Movement Mechanic (in Content Browser) needs to have a Velocity Curve Asset specified!"));
        return;
    }

    const UWorld* world{ GetWorld() };
    if (!world)
    {
        // This happens during shutdown, so don't spam the log
        return;
    }

    CurveLib::VelocityCurveInstance curveInstance{.mMechanic = mechanic->ToNative()};
    
    double minX = 0, maxX = 0;
    mechanic->VelocityCurveAsset->GetTimeRange(minX, maxX);

    // TODO: Properly support curves with negative x content
    curveInstance.mMechanic.mRawAssetDuration = CurveLib::Seconds(maxX - minX);
    
    curveInstance.mSpeedSampler = CurveLib::CreateSamplerXY(mechanic->VelocityCurveAsset);
    CurveLib::StartVelocityCurve(mCurveContext, curveInstance);
}

void UVelocityCurveComponent::StopVelocityCurve(const UCurveMechanic* mechanic)
{
    if (!mechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("StopVelocityCurve: Mechanic pin must be connected"));
        return;
    }

    const CurveLib::CurveInstanceId curveInstanceId = mechanic->GetCurveId();
    CurveLib::StopVelocityCurve(mCurveContext, curveInstanceId);
}

void UVelocityCurveComponent::SoftStopVelocityCurve(const UCurveMechanic* mechanic)
{
    if (!mechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("StopVelocityCurve: Mechanic pin must be connected"));
        return;
    }

    const CurveLib::CurveInstanceId curveInstanceId = mechanic->GetCurveId();
    CurveLib::SoftStopVelocityCurve(mCurveContext, curveInstanceId);
}

void UVelocityCurveComponent::StopAllVelocityCurves()
{
    CurveLib::StopAllVelocityCurves(mCurveContext);
}

void UVelocityCurveComponent::UpdateVelocityCurve(const UCurveMechanic* mechanic, bool updateSpeed, float speedMultiplier, bool updateDirection, FVector direction)
{
    if (!mechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("UpdateVelocityCurve: Mechanic pin must be connected"));
        return;
    }

    // No direct support for std::optional in Unreal, so must use a separate flag (or a sentinel value, which would be inadvisable here)
    const auto optSpeed{ updateSpeed ? std::optional<float>(speedMultiplier) : std::nullopt };
    const auto optDirection{ updateDirection ? std::optional<CurveLib::Float3>(CurveLib::ToFloat3(direction)) : std::nullopt };

    const CurveLib::CurveInstanceId curveInstanceId = mechanic->GetCurveId();
    CurveLib::UpdateVelocityCurve(mCurveContext, curveInstanceId, optSpeed, optDirection);
}

void UVelocityCurveComponent::InputToVelocityCurves(const UCurveMechanic* forwardMechanic, const UCurveMechanic* sideMechanic, const FVector2D& inputAxes, bool isCameraRelative)
{
    if (!forwardMechanic || !sideMechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("InputToVelocityCurves: Must connect forwardCurve and sideCurve pins"));
        return;
    }

    const CurveLib::CurveInstanceId forwardCurveId = forwardMechanic->GetCurveId();
    const CurveLib::CurveInstanceId sideCurveId = sideMechanic->GetCurveId();

    // Convert from 2D to 3D local-space (still in Unreal coordinate system, with Z up).
    // For vertical velocity curves, map the input to a vertically oriented plane. For
    // all other modes, map input to the horizontal plane
    FVector localInputDir;
    if (forwardMechanic->AxisMode == EAxisMode::vertical)
    {
        localInputDir.Set(0, inputAxes.X, -inputAxes.Y);
    }
    else
    {
        localInputDir.Set(inputAxes.X, inputAxes.Y, 0);
    }

    // Unreal input axes are -1 to 1, which is already what we want

    // Preserve vector lengths below 1 to allow for slow analog movement, but disallow length above 1
    localInputDir = localInputDir.GetClampedToMaxSize(1.0);

    FRotator referenceRotation = GetRotationToApply(isCameraRelative);
    
    // Interpret the input vector as being local to the reference rotation, and transform it into
    // worldspace.
    const FVector worldInputDir = referenceRotation.GetInverse().RotateVector(localInputDir);

    if (CurveLib::IsZero(localInputDir.Y))
    {
        CurveLib::SoftStopVelocityCurve(mCurveContext, forwardCurveId);
    }
    else if (!CurveLib::IsCurveRunning(mCurveContext, forwardCurveId))
    {
        // Make sure to call the Unreal-wrapped version of StartVelocityCurve so we get the FloatCurve sampler
        StartVelocityCurve(forwardMechanic);
    }

    if (CurveLib::IsZero(localInputDir.X))
    {
        CurveLib::SoftStopVelocityCurve(mCurveContext, sideCurveId);
    }
    else if (!CurveLib::IsCurveRunning(mCurveContext, sideCurveId))
    {
        StartVelocityCurve(sideMechanic);
    }

    // Update the speedMultipliers of both the velocity curves according to input
    CurveLib::UpdateVelocityCurve(mCurveContext,
                                    forwardCurveId,
                                    localInputDir.Y * forwardMechanic->SpeedMultiplier,
                                    CurveLib::Float3(1, 0, 0));

    CurveLib::UpdateVelocityCurve(mCurveContext,
                                    sideCurveId,
                                    localInputDir.X * sideMechanic->SpeedMultiplier,
                                    std::nullopt);
}

void UVelocityCurveComponent::InputAxisToVelocityCurve(const UCurveMechanic* mechanic, float inputAxis)
{
    if (!mechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("InputAxisToVelocityCurve: Must connect mechanic pin"));
        return;
    }

    const float playheadPosition = CurveLib::CalculateCurveX(mCurveContext, mechanic->GetCurveId());
    if (CurveLib::IsZero(inputAxis) && playheadPosition < mechanic->LoopEndX)
    {
        CurveLib::SoftStopVelocityCurve(mCurveContext, mechanic->GetCurveId());
    }
    else
    {
        if (!CurveLib::IsCurveRunning(mCurveContext, mechanic->GetCurveId()))
        {
            // Call the Unreal-wrapped version of StartVelocityCurve so we inject the FloatCurve sampler
            StartVelocityCurve(mechanic);
        }

        CurveLib::UpdateVelocityCurve(mCurveContext,
            mechanic->GetCurveId(),
            inputAxis * mechanic->SpeedMultiplier,
            CurveLib::Float3(1, 0, 0));
    }
}

float UVelocityCurveComponent::GetMechanicSpeed(const UCurveMechanic* mechanic) const
{
    if (!mechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("GetMechanicSpeed: Must connect mechanic pin"));
        return;
    }
    
    return CurveLib::GetMechanicSpeed(mCurveContext, mechanic->GetCurveId());
}

float UVelocityCurveComponent::GetTotalSpeed() const
{
    return CurveLib::GetTotalSpeed(mCurveContext);
}

bool UVelocityCurveComponent::IsCurveRunning(const UCurveMechanic* mechanic) const
{
    if (!mechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("IsCurveRunning: Mechanic pin must be connected"));
        return false;
    }

    const CurveLib::CurveInstanceId curveInstanceId = mechanic->GetCurveId();
    return CurveLib::IsCurveRunning(mCurveContext, curveInstanceId);
}

bool UVelocityCurveComponent::IsAnyCurveRunning(bool includeLinear, bool includeRotational) const
{
    return CurveLib::IsAnyCurveRunning(mCurveContext, includeLinear, includeRotational);
}

void UVelocityCurveComponent::AttachSpline(const UCurveMechanic* mechanic, const USplineComponent* splineComponent)
{
    if (!splineComponent)
    {
        UE_LOG(CurveLibLog, Error, TEXT("AttachSpline: SplineComponent pin must be connected"));
        return;
    }

    const CurveLib::CurveInstanceId curveInstanceId = mechanic->GetCurveId();
    CurveLib::VelocityCurveInstance* curveInstance = CurveLib::AccessCurveInstance(mCurveContext, curveInstanceId);

    if (!curveInstance)
    {
        UE_LOG(CurveLibLog, Error, TEXT("Velocity curve not found"));
        return;
    }

    curveInstance->mPositionSampler = CurveLib::CreateSplineSampler(splineComponent);
}

/*const void UVelocityCurveComponent::StretchSpline(USplineComponent* splineComponent, FVector startPosition, FVector endPosition, float height, ECoordinateSpace coordinateSpace)
{
    if (!splineComponent)
    {
        UE_LOG(CurveLibLog, Error, TEXT("StretchSpline: SplineComponent pin must be connected"));
        return;
    }
    //splineComponent->ApplyComponentInstanceData(FSplineComponentInstanceData
    //const FBoxSphereBounds bounds = splineComponent->CalcBounds()
    FSpline alteredSpline = splineComponent->SplineCurves.
    
        splineComponent->SetSpline(alteredSpline);
}*/

void UVelocityCurveComponent::SetLocation(FVector location)
{
    CurveLib::SetPosition(mCurveContext, location.X, location.Y, location.Z);
}

FVector UVelocityCurveComponent::GetVelocity()
{
    return ToFVector(mCurveContext.mOutput.mVelocity);
}

FVector UVelocityCurveComponent::GetAngularVelocity()
{
    return ToFVector(mCurveContext.mOutput.mAngularVelocity);
}

void UVelocityCurveComponent::ShowCurveDebugger(bool show)
{
    if(show)
    {
        // Ensure we are in a valid world context
        UWorld* world = GetWorld();
        if (!world)
        {
            UE_LOG(CurveLibLog, Error, TEXT("Cannot create UVelocityCurveDebugger: World is null."));
            return;
        }

        if (!mCurveDebugger)
        {
            if (APlayerController* owner = UGameplayStatics::GetPlayerController(world, 0))
            {
                mCurveDebugger = CreateWidget<UVelocityCurveDebugger>(owner, UVelocityCurveDebugger::StaticClass());
            }
        }

        if (mCurveDebugger)
        {
            mCurveDebugger->AddToViewport();
            mCurveDebugger->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            UE_LOG(CurveLibLog, Error, TEXT("UVelocityCurveDebugger could not be created."));
        }
    }
    else
    {
        if (mCurveDebugger)
        {
            mCurveDebugger->RemoveFromParent();
        }
    }
}

FString UVelocityCurveComponent::GetCurveOutputAsDebugString(const UCurveMechanic* mechanic) const
{
    if (!mechanic)
    {
        UE_LOG(CurveLibLog, Error, TEXT("GetCurveOutputAsDebugString: Mechanic pin must be connected"));
        return "GetCurveOutputAsDebugString: Mechanic pin must be connected";
    }

    const auto curveId = mechanic->GetCurveId();
    const auto linearIter = mCurveContext.mLinearCurves.find(curveId);
    if (linearIter != mCurveContext.mLinearCurves.end())
    {
        //return FString(std::to_string(linearIter->second.mDistanceAccumulator.GetLatestSample().Y).c_str());
        return FString(linearIter->second.mOutput.ToString().c_str());
    }

    const auto rotationIter = mCurveContext.mRotationCurves.find(curveId);
    if (rotationIter != mCurveContext.mRotationCurves.end())
    {
        return FString(rotationIter->second.mOutput.ToString().c_str());
    }

    return "Curve not found";
}

FString UVelocityCurveComponent::GetFinalOutputAsDebugString() const
{
    return UTF8_TO_TCHAR(mCurveContext.mOutput.ToString().c_str());
}

FString UVelocityCurveComponent::GetRunningCurvesAsDebugString() const
{
    std::stringstream ss;

    auto joinedRefs = CurveLib::GetJoinedContainerReferences(mCurveContext.mLinearCurves, mCurveContext.mRotationCurves);
    auto allCurveInstances = joinedRefs | std::views::join;
    for (const auto& curvePair : allCurveInstances)
    {
        const CurveLib::VelocityCurveInstance& instance = curvePair.second;

        const std::string debugName = instance.mMechanic.mDebugName ? *instance.mMechanic.mDebugName : "Unnamed";
        ss << instance.ToString();
    }

    return UTF8_TO_TCHAR(ss.str().c_str());
}

int32_t UVelocityCurveComponent::CurveNameToInstanceId(const FString& curveName) const
{
    return GetTypeHash(curveName);
}

CurveLib::VelocityCurveContext& UVelocityCurveComponent::AccessCurveContext()
{
    return mCurveContext;
}

const CurveLib::VelocityCurveContext& UVelocityCurveComponent::GetCurveContext() const
{
    return mCurveContext;
}

FRotator UVelocityCurveComponent::GetRotationToApply(bool isCameraRelative)
{
    if (isCameraRelative)
    {
        if (APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            // Get the rotation from the Player Camera Manager (most accurate for the viewport view)
            if (APlayerCameraManager* cameraManager = playerController->PlayerCameraManager)
            {
                return cameraManager->GetCameraRotation();
            }
            else
            {
                // Fall back to the control rotation
                return playerController->GetControlRotation();
            }
        }
    }
    else
    {
        // Character-relative controls (sometimes called "tank controls" by players)
        if (const auto* owner = GetOwner())
        {
            return owner->GetActorRotation();
        }
    }

    return {};
}

void UVelocityCurveComponent::SendVelocityToUnreal(const CurveLib::Float3& velocity, const CurveLib::Float3& angularVelocity)
{
    const auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    auto* primitiveComponent = owner->GetComponentByClass<UPrimitiveComponent>();
    if (!primitiveComponent)
    {
        UE_LOG(CurveLibLog, Error, TEXT("Primitive component not found. Either disable VelocityCurveComponent::OutputVelocity or enable SimulatePhysics on your actor's root component"));
        return;
    }

    primitiveComponent->SetAllPhysicsLinearVelocity(CurveLib::ToFVector(velocity));
    primitiveComponent->SetAllPhysicsAngularVelocityInDegrees(CurveLib::ToFVector(angularVelocity));
}

#endif // #if defined(__UNREAL__)