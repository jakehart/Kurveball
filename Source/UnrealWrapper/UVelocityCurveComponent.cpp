// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#if defined(__UNREAL__)

#include "UnrealWrapper/UVelocityCurveComponent.h"

#include <Blueprint/UserWidget.h>
#include <Engine/Engine.h>
#include <Components/SplineComponent.h>
#include <GameFramework/PlayerController.h>
#include <Kismet/GameplayStatics.h>

#include <sstream>

#include "UnrealWrapper/KurveballLog.h"
#include "ContainerUtils.h"
#include "MathUtils.h"
#include "UnrealWrapper/EBlendType.h"
#include "UnrealWrapper/USensorComponent.h"
#include "UnitTypes.h"
#include "VelocityCurveApi.h"
#include "VelocityCurveInstance.h"
#include "VelocityCurvePlayback.h"

#include "UnrealWrapper/UVelocityCurveDebugger.h"

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

    // Tell Kurveball about any position change that happened due to collision or simulated physics
    const FVector position = owner->GetActorLocation();
    Kurveball::SetPosition(mCurveContext, position.X, position.Y, position.Z);
    
    Kurveball::TickPlayback(mCurveContext, Kurveball::Seconds(absoluteTime));

    // The velocity curve context doesn't care which world units we use as long as we're consistent.
    // Here we're using Unreal units (cm) directly.
    const FVector newPosition = Kurveball::ToFVector(mCurveContext.mOutput.mPosition);
    // Rotation always uses degrees
    const FRotator newRotation = FRotator(mCurveContext.mOutput.mRotation.Y, mCurveContext.mOutput.mRotation.Z, mCurveContext.mOutput.mRotation.X);
    
    if (RespectCollision)
    {
        SendVelocityToUnreal(mCurveContext.mOutput.mVelocity, mCurveContext.mOutput.mAngularVelocity);
    }
    else
    {
        owner->TeleportTo(newPosition, newRotation, false, true);
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

    // Unreal uses Z+ up. Kurveball needs to know this so that it can mask off the
    // correct axes when you use eAxisMode::vertical or eAxisMode::horizontal.
    Kurveball::SetVerticalAxis(mCurveContext, Axis::Z);

    const FRotator rotation = owner->GetActorRotation();
    const FVector euler = rotation.Euler();
    Kurveball::SetRotation(mCurveContext, euler.X, euler.Y, euler.Z);

    // Position is updated in TickComponent
}

void UVelocityCurveComponent::StartVelocityCurve(const UCurveMechanic* mechanic)
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("ERROR: StartVelocityCurve (in Blueprint) needs to have a Movement Mechanic Asset specified!"));
        return;
    }

    if (!mechanic->VelocityCurveAsset)
    {
        UE_LOG(KurveballLog, Error, TEXT("ERROR: Movement Mechanic (in Content Browser) needs to have a Velocity Curve Asset specified!"));
        return;
    }

    const UWorld* world{ GetWorld() };
    if (!world)
    {
        // This happens during shutdown, so don't spam the log
        return;
    }

    Kurveball::VelocityCurveInstance curveInstance{.mMechanic = mechanic->ToNative()};
    
    double minX = 0, maxX = 0;
    mechanic->VelocityCurveAsset->GetTimeRange(minX, maxX);

    // TODO: Properly support curves with negative x content
    curveInstance.mMechanic.mRawAssetDuration = Kurveball::Seconds(maxX - minX);
    
    curveInstance.mSpeedSampler = Kurveball::CreateSamplerXY(mechanic->VelocityCurveAsset);
    Kurveball::StartVelocityCurve(mCurveContext, curveInstance);
}

void UVelocityCurveComponent::StopVelocityCurve(const UCurveMechanic* mechanic)
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("StopVelocityCurve: Mechanic pin must be connected"));
        return;
    }

    const Kurveball::CurveInstanceID curveInstanceID = mechanic->GetCurveID();
    Kurveball::StopVelocityCurve(mCurveContext, curveInstanceID);
}

void UVelocityCurveComponent::SoftStopVelocityCurve(const UCurveMechanic* mechanic)
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("StopVelocityCurve: Mechanic pin must be connected"));
        return;
    }

    const Kurveball::CurveInstanceID curveInstanceID = mechanic->GetCurveID();
    Kurveball::SoftStopVelocityCurve(mCurveContext, curveInstanceID);
}

void UVelocityCurveComponent::StopAllVelocityCurves()
{
    Kurveball::StopAllVelocityCurves(mCurveContext);
}

void UVelocityCurveComponent::SeekToX(const UCurveMechanic* mechanic, float curveXCoordinate)
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("SeekToX: Mechanic pin must be connected"));
        return;
    }

    Kurveball::SeekToX(mCurveContext, mechanic->GetCurveID(), curveXCoordinate);
}

void UVelocityCurveComponent::UpdateVelocityCurve(const UCurveMechanic* mechanic, bool updateSpeed, float speedMultiplier, bool updateDirection, FVector direction)
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("UpdateVelocityCurve: Mechanic pin must be connected"));
        return;
    }

    // No direct support for std::optional in Unreal, so must use a separate flag (or a sentinel value, which would be inadvisable here)
    const auto optSpeed{ updateSpeed ? std::optional<float>(speedMultiplier) : std::nullopt };
    const auto optDirection{ updateDirection ? std::optional<Kurveball::Float3>(Kurveball::ToFloat3(direction)) : std::nullopt };

    const Kurveball::CurveInstanceID curveInstanceID = mechanic->GetCurveID();
    Kurveball::UpdateVelocityCurve(mCurveContext, curveInstanceID, optSpeed, optDirection);
}

void UVelocityCurveComponent::InputToVelocityCurves(const UCurveMechanic* forwardMechanic, const UCurveMechanic* sideMechanic, const FVector2D& inputAxes, bool isCameraRelative)
{
    if (!forwardMechanic || !sideMechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("InputToVelocityCurves: Must connect forwardCurve and sideCurve pins"));
        return;
    }

    if (!isCameraRelative)
    {
        // Character-relative "tank" controls. No need for anything fancy; just pass the
        // input axes through
        InputAxisToVelocityCurve(forwardMechanic, inputAxes.Y);
        InputAxisToVelocityCurve(sideMechanic, inputAxes.X);
        return;
    }

    const auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    const Kurveball::CurveInstanceID forwardCurveID = forwardMechanic->GetCurveID();
    const Kurveball::CurveInstanceID sideCurveID = sideMechanic->GetCurveID();

    const bool isVertical = forwardMechanic->AxisMode == EAxisMode::vertical;

    // Convert from 2D to 3D local-space (still in Unreal coordinate system, with Z up).
    // For vertical velocity curves, map the input to a vertically oriented plane. For
    // all other modes, map input to the horizontal plane
    FVector localInputDir{};
    if (isVertical)
    {
        localInputDir.Set(0, inputAxes.Y, inputAxes.X);
    }
    else
    {
        localInputDir.Set(-inputAxes.Y, inputAxes.X, 0);
    }

    // Unreal input axes are -1 to 1, which is already what we want

    // Preserve vector lengths below 1 to allow for slow analog movement, but disallow length above 1
    localInputDir = localInputDir.GetClampedToMaxSize(1.0);

    const FRotator cameraRotation = GetCameraRotation();

    // Transform the input from camera-space to world-space as a middleman
    const FVector worldInputDir = cameraRotation.GetInverse().RotateVector(localInputDir);
        
    // Transform into character-local space
    localInputDir = owner->GetActorRotation().RotateVector(worldInputDir);

    InputAxisToVelocityCurve(forwardMechanic, isVertical ? localInputDir.Z : -localInputDir.X);
    InputAxisToVelocityCurve(sideMechanic, localInputDir.Y);
}

void UVelocityCurveComponent::InputAxisToVelocityCurve(const UCurveMechanic* mechanic, float inputAxis)
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("InputAxisToVelocityCurve: Must connect mechanic pin"));
        return;
    }

    const Kurveball::CurveInstanceID curveID = mechanic->GetCurveID();

    const float playheadPosition = Kurveball::CalculateCurveX(mCurveContext, curveID);
    
    if (Kurveball::IsZero(inputAxis))
    {
        // Player released the controls, so seek to the outro of the curve, or to its end if there's no outro
        Kurveball::SoftStopVelocityCurve(mCurveContext, curveID);
    }
    /*else if (playheadPosition > mechanic->LoopEndX + Kurveball::sFloatMinDenormal)
    {
        // Player started moving the controls again while the curve is winding down.
        // Seek to the beginning
        Kurveball::SeekToX(mCurveContext, curveID, 0.f);
    }*/
    else
    {
        // Nonzero input
        if (!Kurveball::IsCurveRunning(mCurveContext, curveID))
        {
            // Call the Unreal-wrapped version of StartVelocityCurve so we inject the FloatCurve sampler
            StartVelocityCurve(mechanic);
        }

        Kurveball::Float3 direction{1, 0, 0}; // forward
        if (mechanic->CoordinateSpace == ECoordinateSpace::world)
        {
            direction = direction.LocalToWorldDirection(mCurveContext.mOutput.mRotation);
        }

        Kurveball::UpdateVelocityCurve(mCurveContext,
            curveID,
            inputAxis * mechanic->SpeedMultiplier,
            direction);
    }
}

float UVelocityCurveComponent::GetMechanicSpeed(const UCurveMechanic* mechanic) const
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("GetMechanicSpeed: Must connect mechanic pin"));
        return 0.f;
    }
    
    return Kurveball::GetMechanicSpeed(mCurveContext, mechanic->GetCurveID());
}

float UVelocityCurveComponent::GetTotalSpeed() const
{
    return Kurveball::GetTotalSpeed(mCurveContext);
}

bool UVelocityCurveComponent::IsCurveRunning(const UCurveMechanic* mechanic) const
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("IsCurveRunning: Mechanic pin must be connected"));
        return false;
    }

    const Kurveball::CurveInstanceID curveInstanceID = mechanic->GetCurveID();
    return Kurveball::IsCurveRunning(mCurveContext, curveInstanceID);
}

bool UVelocityCurveComponent::IsAnyCurveRunning(bool includeLinear, bool includeRotational) const
{
    return Kurveball::IsAnyCurveRunning(mCurveContext, includeLinear, includeRotational);
}

void UVelocityCurveComponent::AttachSpline(const UCurveMechanic* mechanic, const USplineComponent* splineComponent, float desiredHeight)
{
    if (!splineComponent || splineComponent->GetSplineLength() < Kurveball::sFloatEpsilon)
    {
        UE_LOG(KurveballLog, Error, TEXT("UVelocityCurveComponent received null or empty spline"));
        return;
    }

    const Kurveball::CurveInstanceID curveInstanceID = mechanic->GetCurveID();
    Kurveball::VelocityCurveInstance* curveInstance = Kurveball::AccessCurveInstance(mCurveContext, curveInstanceID);

    if (!curveInstance)
    {
        UE_LOG(KurveballLog, Error, TEXT("Velocity curve not found"));
        return;
    }

    curveInstance->mPositionSampler = Kurveball::CreateUnrealSplineSampler(splineComponent, desiredHeight);
}

void UVelocityCurveComponent::GenerateParabolicSpline(const UCurveMechanic* mechanic, float heightCm, FVector destination)
{
    auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    const Kurveball::CurveInstanceID curveInstanceID = mechanic->GetCurveID();
    Kurveball::VelocityCurveInstance* curveInstance = Kurveball::AccessCurveInstance(mCurveContext, curveInstanceID);

    if (!curveInstance)
    {
        UE_LOG(KurveballLog, Error, TEXT("Velocity curve not found"));
        return;
    }

    USplineComponent* splineComponent = CreateDefaultSubobject<USplineComponent>("MySpline", true);

    static const FVector sOneScale(1.f, 1.f, 1.f);

    FSplinePoint beginning;
    beginning.Position = owner->GetActorLocation();
    beginning.Rotation = owner->GetActorRotation();
    beginning.Scale = sOneScale;

    FSplinePoint peak;
    peak.Position = (beginning.Position + destination) / 2.f;
    peak.Position.Z += heightCm;
    peak.Rotation = beginning.Rotation;
    peak.Scale = sOneScale;

    FSplinePoint end;
    end.Position = destination;
    end.Rotation = beginning.Rotation;
    end.Scale = sOneScale;

    splineComponent->AddPoint(beginning, false);
    splineComponent->AddPoint(peak, false);
    splineComponent->AddPoint(end, false);
    splineComponent->UpdateSpline();

    curveInstance->mPositionSampler = Kurveball::CreateUnrealSplineSampler(splineComponent);
}

void UVelocityCurveComponent::SetLocation(FVector location)
{
    Kurveball::SetPosition(mCurveContext, location.X, location.Y, location.Z);
}

FVector UVelocityCurveComponent::GetVelocity()
{
    return ToFVector(mCurveContext.mOutput.mVelocity);
}

FVector UVelocityCurveComponent::GetLocalVelocity()
{
    return ToFVector(mCurveContext.mOutput.mVelocity.WorldToLocalDirection(mCurveContext.mOutput.mRotation));
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
            UE_LOG(KurveballLog, Error, TEXT("Cannot create UVelocityCurveDebugger: World is null."));
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
            UE_LOG(KurveballLog, Error, TEXT("UVelocityCurveDebugger could not be created."));
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
        UE_LOG(KurveballLog, Error, TEXT("GetCurveOutputAsDebugString: Mechanic pin must be connected"));
        return "GetCurveOutputAsDebugString: Mechanic pin must be connected";
    }

    const auto curveID = mechanic->GetCurveID();
    const auto linearIter = mCurveContext.mLinearCurves.find(curveID);
    if (linearIter != mCurveContext.mLinearCurves.end())
    {
        //return FString(std::to_string(linearIter->second.mDistanceAccumulator.GetLatestSample().Y).c_str());
        return FString(linearIter->second.mOutput.ToString().c_str());
    }

    const auto rotationIter = mCurveContext.mRotationCurves.find(curveID);
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

    auto joinedRefs = Kurveball::GetJoinedContainerReferences(mCurveContext.mLinearCurves, mCurveContext.mRotationCurves);
    auto allCurveInstances = joinedRefs | std::views::join;
    for (const auto& curvePair : allCurveInstances)
    {
        const Kurveball::VelocityCurveInstance& instance = curvePair.second;

        const std::string debugName = instance.mMechanic.mDebugName ? *instance.mMechanic.mDebugName : "Unnamed";
        ss << instance.ToString();
    }

    return UTF8_TO_TCHAR(ss.str().c_str());
}

int32_t UVelocityCurveComponent::CurveNameToInstanceID(const FString& curveName) const
{
    return GetTypeHash(curveName);
}

void UVelocityCurveComponent::DefineCurveXFunction(const UCurveMechanic* mechanic, const FCurveXSampler& xSampler)
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("DefineCurveXFunction: Mechanic port must be connected"));
        return;
    }

    if (auto* curveInstance = Kurveball::AccessCurveInstance(mCurveContext, mechanic->GetCurveID()))
    {
        curveInstance->mXSampler.emplace([&]() -> float
            {
                return xSampler.Execute();
            });
    }
}

void UVelocityCurveComponent::Crossfade(UCurveMechanic* from, UCurveMechanic* to, EBlendType blendType, float duration)
{
    if (!from || !to)
    {
        UE_LOG(KurveballLog, Error, TEXT("Crossfade: From and To must be connected"));
        return;
    }

    Kurveball::Crossfade(mCurveContext, from->GetCurveID(), to->GetCurveID(), static_cast<Kurveball::BlendType>(blendType), Kurveball::Seconds(duration));
}

void UVelocityCurveComponent::Blend(UCurveMechanic* mechanic, EBlendType blendType, float duration, bool isBlendIn)
{
    if (!mechanic)
    {
        UE_LOG(KurveballLog, Error, TEXT("Blend: Mechanic must be connected"));
        return;
    }

    Kurveball::Blend(mCurveContext, mechanic->GetCurveID(), static_cast<Kurveball::BlendType>(blendType), Kurveball::Seconds(duration), isBlendIn);
}

Kurveball::VelocityCurveContext& UVelocityCurveComponent::AccessCurveContext()
{
    return mCurveContext;
}

const Kurveball::VelocityCurveContext& UVelocityCurveComponent::GetCurveContext() const
{
    return mCurveContext;
}

void UVelocityCurveComponent::TransferCurve(UCurveMechanic* fromMechanic, UCurveMechanic* toMechanic, EBlendType blendType, float blendDuration, bool startToCurveIfNotFound)
{
    Kurveball::TransferCurve(mCurveContext, fromMechanic->ToNative(), toMechanic->ToNative(), static_cast<Kurveball::BlendType>(blendType), Kurveball::Seconds(blendDuration), startToCurveIfNotFound);
}

FRotator UVelocityCurveComponent::GetCameraRotation()
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

    return {};
}

void UVelocityCurveComponent::SendVelocityToUnreal(const Kurveball::Float3& velocity, const Kurveball::Float3& angularVelocity)
{
    const auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    UPrimitiveComponent* primitiveComponent = nullptr;
    if (owner->GetRootComponent())
    {
        primitiveComponent = Cast<UPrimitiveComponent>(owner->GetRootComponent());
    }
    else
    {
        primitiveComponent = owner->FindComponentByClass<UPrimitiveComponent>();
    }

    if (!primitiveComponent)
    {
        UE_LOG(KurveballLog, Error, TEXT("To use RespectCollision=true, you must add a physics volume to your actor and enable SimulatePhysics"));
        return;
    }

    if (!primitiveComponent->IsSimulatingPhysics())
    {
        primitiveComponent->SetSimulatePhysics(true);
    }

    primitiveComponent->SetPhysicsLinearVelocity(Kurveball::ToFVector(velocity));
    primitiveComponent->SetPhysicsAngularVelocityInDegrees(Kurveball::ToFVector(angularVelocity));
}

#endif // #if defined(__UNREAL__)