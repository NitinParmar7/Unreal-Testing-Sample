#pragma once

#include "CoreMinimal.h"

/** Pure C++ damage maths. No UObjects, so it can be tested without a world. */
struct UNREALTESTINGSAMPLE_API FDamageCalculator
{
    /** Returns the remaining health after armor-reduced damage is applied. Never negative. */
    static float Apply(float Health, float Damage, float Armor);
};
