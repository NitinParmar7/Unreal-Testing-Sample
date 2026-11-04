#include "FireballEffects.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "SampleAttributeSet.h"
#include "SampleGameplayTags.h"

UFireballCostEffect::UFireballCostEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo Modifier;
    Modifier.Attribute = USampleAttributeSet::GetManaAttribute();
    Modifier.ModifierOp = EGameplayModOp::Additive;
    Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-25.f));
    Modifiers.Add(Modifier);
}

UFireballCooldownEffect::UFireballCooldownEffect()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(3.f));

    // The cooldown *is* the tag: while it is present, the ability cannot be activated.
    UTargetTagsGameplayEffectComponent& TagsComponent = FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
    FInheritedTagContainer TagChanges;
    TagChanges.Added.AddTag(SampleTags::Cooldown_Fireball.GetTag());
    TagsComponent.SetAndApplyTargetTagChanges(TagChanges);
}
