#include "DamageCalculator.h"

float FDamageCalculator::Apply(float Health, float Damage, float Armor)
{
    const float Reduced = FMath::Max(0.f, Damage - Armor);
    return FMath::Max(0.f, Health - Reduced);
}
