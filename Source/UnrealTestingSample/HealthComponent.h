#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "HealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHealthDepleted);

/** Health with delayed, timer-driven regeneration. The subject of the timer tests. */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALTESTINGSAMPLE_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Health") float MaxHealth = 100.f;
    /** Seconds after taking damage before regeneration starts. */
    UPROPERTY(EditAnywhere, Category = "Health") float RegenDelay = 5.f;
    /** Health restored once per second while regenerating. */
    UPROPERTY(EditAnywhere, Category = "Health") float RegenPerSecond = 10.f;

    FOnHealthDepleted OnDied;

    float GetHealth() const { return Health; }
    bool IsDead() const { return Health <= 0.f; }

    void TakeDamage(float Amount);

protected:
    virtual void BeginPlay() override;

private:
    float Health = 0.f;
    FTimerHandle RegenDelayHandle;
    FTimerHandle RegenTickHandle;

    void StartRegen();
    void RegenTick();
};
