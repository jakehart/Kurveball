// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include "Kurveball/VelocityCurveOutput.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "USensorComponent.generated.h"

USTRUCT(BlueprintType)
struct FSensorDescription
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector LocalStartPosition = {};
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector LocalDirection = {};
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TraceLength = 1.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RadiusCm = 50.f;

    // If true, this sensor will automatically be treated as a collision (stopping motion in that direction)
    // when it hits anything and VelocityCurveComponent has RespectCollision enabled.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsAutoCollide = true;

private:
    FHitResult Result{};
    bool IsWaitingForResult = false;
    friend class USensorComponent;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CURVEDEMO_API USensorComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
public:    
    USensorComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Sensors")
    FHitResult GetSensorResult(FName sensorName) const;

    UFUNCTION(BlueprintCallable, Category = "Sensors")
    bool HasSensorResult(FName sensorName) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FSensorDescription> SensorDescriptions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool ShowDebug = false;
private:
    void SanitizeSensorDescription(FSensorDescription& ioSensor);

    void SyncTraceSensor(FSensorDescription& ioSensor);

    FTransform LastTickTransform{};
};