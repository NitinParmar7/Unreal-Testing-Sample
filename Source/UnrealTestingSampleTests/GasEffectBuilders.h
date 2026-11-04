#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "UObject/Package.h"

namespace GasTest
{
    inline UGameplayEffect* MakeInstant(const FGameplayAttribute& Attribute, float Delta)
    {
        UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage());
        Effect->DurationPolicy = EGameplayEffectDurationType::Instant;

        FGameplayModifierInfo Modifier;
        Modifier.Attribute = Attribute;
        Modifier.ModifierOp = EGameplayModOp::Additive;
        Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Delta));
        Effect->Modifiers.Add(Modifier);
        return Effect;
    }

    inline UGameplayEffect* MakeTimed(const FGameplayAttribute& Attribute, float Delta, float Seconds)
    {
        UGameplayEffect* Effect = MakeInstant(Attribute, Delta);
        Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
        Effect->DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Seconds));
        return Effect;
    }

    inline UGameplayEffect* MakePeriodic(const FGameplayAttribute& Attribute, float DeltaPerTick, float PeriodSeconds, float DurationSeconds)
    {
        UGameplayEffect* Effect = MakeTimed(Attribute, DeltaPerTick, DurationSeconds);
        Effect->Period = FScalableFloat(PeriodSeconds);
        return Effect;
    }

    inline FActiveGameplayEffectHandle Apply(UAbilitySystemComponent* Asc, const UGameplayEffect* Effect)
    {
        return Asc->ApplyGameplayEffectToSelf(Effect, 1.f, Asc->MakeEffectContext());
    }
}
