#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "FireballEffects.generated.h"

UCLASS()
class UNREALTESTINGSAMPLE_API UFireballCostEffect : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UFireballCostEffect();
};

UCLASS()
class UNREALTESTINGSAMPLE_API UFireballCooldownEffect : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UFireballCooldownEffect();
};
