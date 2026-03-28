#if defined(__UNREAL__)
#include "USensorComponent.h"

#include "DrawDebugHelpers.h"

#include "Kurveball/UnitTypes.h"
#include "Kurveball/VelocityCurveOutput.h"

USensorComponent::USensorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USensorComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize to current position so that the first frame is correct
    if (const auto* owner = GetOwner())
    {
        LastTickTransform = owner->GetTransform();
    }


    for (auto& [sensorName, sensor] : SensorDescriptions)
    {
        SanitizeSensorDescription(sensor);
    }
}
    
void USensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    for (auto& [sensorName, sensor] : SensorDescriptions)
    {
        if (!sensor.IsEnabled || sensor.IsWaitingForResult)
        {
            continue;
        }

        SyncTraceSensor(sensor);
    }

    if (const auto* owner = GetOwner())
    {
        LastTickTransform = owner->GetTransform();
    }
}

FHitResult USensorComponent::GetSensorResult(FName sensorName) const
{
    const auto sensor = SensorDescriptions.Find(sensorName);
    if (!sensor || !sensor->IsEnabled || sensor->IsWaitingForResult)
    {
        return {};
    }

    return sensor->Result;
}

bool USensorComponent::HasSensorResult(FName sensorName) const
{
    const auto sensor = SensorDescriptions.Find(sensorName);
    if (!sensor || !sensor->IsEnabled || sensor->IsWaitingForResult)
    {
        return false;
    }

    return true;
}

void USensorComponent::SanitizeSensorDescription(FSensorDescription& ioSensor)
{
    if (ioSensor.LocalDirection.IsNearlyZero())
    {
        ioSensor.LocalDirection.Set(1, 0, 0);
    }
    else
    {
        ioSensor.LocalDirection.Normalize(Kurveball::sFloatEpsilon);
    }
}

void USensorComponent::SyncTraceSensor(FSensorDescription& ioSensor)
{
    const auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    auto* world = GetWorld();
    if (!world)
    {
        return;
    }

    const auto& currentTransform = owner->GetTransform();

    // Using last frame's position as the starting point to ensure that low FPS don't cause
    // us to be able to go through objects.
    const FVector worldStartPosition = LastTickTransform.TransformPosition(ioSensor.LocalStartPosition);
    const FVector worldDirection = currentTransform.TransformVector(ioSensor.LocalDirection);

    // The end position is calculated from *this* frame's position, so that we get a smooth sweep from last frame's
    // position to this frame's sensor endpoint
    const FVector worldEndPosition = currentTransform.GetLocation() + worldDirection * ioSensor.TraceLength;

    FHitResult hit{};
    FCollisionQueryParams params{};
    params.AddIgnoredActor(owner);

    bool bHit = GetWorld()->SweepSingleByChannel(
        hit,
        worldStartPosition,
        worldEndPosition,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(ioSensor.RadiusCm),
        params
    );

    ioSensor.Result = hit;
    ioSensor.IsWaitingForResult = false;

    if (ShowDebug)
    {
        DrawDebugLine(world, worldStartPosition, worldEndPosition, FColor::Red, false);
    }
}

#endif // #if defined(__UNREAL__)