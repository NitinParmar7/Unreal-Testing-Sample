#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SampleAttributeSet.generated.h"

#define SAMPLE_ATTRIBUTE_ACCESSORS(ClassName, PropertyName)          \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)       \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)                     \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)                     \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class UNREALTESTINGSAMPLE_API USampleAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UPROPERTY() FGameplayAttributeData Health;
    SAMPLE_ATTRIBUTE_ACCESSORS(USampleAttributeSet, Health)

    UPROPERTY() FGameplayAttributeData MaxHealth;
    SAMPLE_ATTRIBUTE_ACCESSORS(USampleAttributeSet, MaxHealth)

    UPROPERTY() FGameplayAttributeData Mana;
    SAMPLE_ATTRIBUTE_ACCESSORS(USampleAttributeSet, Mana)

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
