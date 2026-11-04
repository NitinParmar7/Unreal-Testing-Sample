#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "SampleAttributeSet.h"

namespace GasTest
{
    struct FFixture
    {
        AActor* Actor = nullptr;
        UAbilitySystemComponent* Asc = nullptr;
        const USampleAttributeSet* Attributes = nullptr;
    };

    inline FFixture Spawn(UWorld* World, float MaxHealth = 100.f, float Mana = 50.f)
    {
        FFixture Out;
        Out.Actor = World->SpawnActor<AActor>();

        Out.Asc = NewObject<UAbilitySystemComponent>(Out.Actor);
        Out.Attributes = Out.Asc->AddAttributeSetSubobject(NewObject<USampleAttributeSet>(Out.Actor));

        Out.Actor->AddInstanceComponent(Out.Asc);
        Out.Asc->RegisterComponent();
        Out.Asc->InitAbilityActorInfo(Out.Actor, Out.Actor);
        Out.Actor->DispatchBeginPlay();

        // Max health first: Health is clamped against it.
        Out.Asc->SetNumericAttributeBase(USampleAttributeSet::GetMaxHealthAttribute(), MaxHealth);
        Out.Asc->SetNumericAttributeBase(USampleAttributeSet::GetHealthAttribute(), MaxHealth);
        Out.Asc->SetNumericAttributeBase(USampleAttributeSet::GetManaAttribute(), Mana);
        return Out;
    }

    inline float Get(const FFixture& F, const FGameplayAttribute& Attribute)
    {
        return F.Asc->GetNumericAttribute(Attribute);
    }
}
