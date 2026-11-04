#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FireballAbility.generated.h"

UCLASS()
class UNREALTESTINGSAMPLE_API UFireballAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UFireballAbility();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;
};
