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

        AsyncTraceSensor(sensor);
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

void USensorComponent::AsyncTraceSensor(FSensorDescription& ioSensor)
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

    FTraceDelegate traceCompleteHandler;
    traceCompleteHandler.BindLambda([&](const FTraceHandle& handle, FTraceDatum& traceData)
        {
            ioSensor.Result = {};
            ioSensor.IsWaitingForResult = false;

            if (traceData.OutHits.Num() < 1)
            {
                return;
            }

            const auto* hit = traceData.OutHits.GetData();
            if (!hit)
            {
                return;
            }

            // Store the result by copy so we don't have to worry about object lifetime
            ioSensor.Result = *hit;
        });

    // Using last frame's position as the starting point to ensure that low FPS don't cause
    // us to be able to go through objects.
    const FVector worldStartPosition = LastTickTransform.TransformPosition(ioSensor.LocalStartPosition);
    const FVector worldDirection = owner->GetTransform().TransformVector(ioSensor.LocalDirection);

    // The end position is calculated from *this* frame's position to ensure that we reach the end of the trace
    const FVector worldEndPosition = worldStartPosition + worldDirection * ioSensor.TraceLength;

    FCollisionQueryParams params;
    params.AddIgnoredActor(owner);

    // TODO: When I set this to true, the async handler never meaningfully sets it back to false again.
    // I think this is because of a race between this line and the handler above. Need thread safety/mutex here
    //ioSensor.IsWaitingForResult = true;

    DrawDebugLine(world, worldStartPosition, worldEndPosition, FColor::Red, false);

    world->AsyncSweepByChannel(
        EAsyncTraceType::Single,
        worldStartPosition,
        worldEndPosition,
        FQuat::Identity,
        ECollisionChannel::ECC_WorldDynamic,        
        FCollisionShape::MakeSphere(ioSensor.RadiusCm), // TODO: reuse shape
        params,
        FCollisionResponseParams::DefaultResponseParam,
        &traceCompleteHandler
    );
}

#endif // #if defined(__UNREAL__)