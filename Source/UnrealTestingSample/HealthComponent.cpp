#include "HealthComponent.h"
#include "Engine/World.h"

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
}

void UHealthComponent::TakeDamage(float Amount)
{
    if (IsDead())
    {
        return;
    }

    Health = FMath::Max(0.f, Health - Amount);

    FTimerManager& TimerManager = GetWorld()->GetTimerManager();
    TimerManager.ClearTimer(RegenDelayHandle);
    TimerManager.ClearTimer(RegenTickHandle);

    if (IsDead())
    {
        OnDied.Broadcast();
        return;
    }

    TimerManager.SetTimer(RegenDelayHandle, this, &UHealthComponent::StartRegen, RegenDelay, false);
}

void UHealthComponent::StartRegen()
{
    GetWorld()->GetTimerManager().SetTimer(
        RegenTickHandle, this, &UHealthComponent::RegenTick, 1.f, true);
}

void UHealthComponent::RegenTick()
{
    Health = FMath::Min(MaxHealth, Health + RegenPerSecond);
    if (Health >= MaxHealth)
    {
        GetWorld()->GetTimerManager().ClearTimer(RegenTickHandle);
    }
}
