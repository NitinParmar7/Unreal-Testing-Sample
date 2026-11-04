#include "FireballAbility.h"
#include "FireballEffects.h"
#include "SampleGameplayTags.h"

UFireballAbility::UFireballAbility()
{
    CostGameplayEffectClass = UFireballCostEffect::StaticClass();
    CooldownGameplayEffectClass = UFireballCooldownEffect::StaticClass();
    ActivationBlockedTags.AddTag(SampleTags::State_Stunned.GetTag());
}

void UFireballAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    // CommitAbility spends the cost and starts the cooldown.
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCancelled*/ true);
        return;
    }

    // Spawn the projectile here in the real game. The tests only care about cost, cooldown and gating.
    EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCancelled*/ false);
}
