#include "Misc/AutomationTest.h"
#include "DamageCalculator.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDamageCalculatorTest,
    "UnrealTestingSample.Combat.DamageCalculator.ArmorReducesDamage",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FDamageCalculatorTest::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("Armor reduces damage"), FDamageCalculator::Apply(100.f, 30.f, 10.f), 80.f);
    TestEqual(TEXT("Armor above damage deals nothing"), FDamageCalculator::Apply(100.f, 5.f, 10.f), 100.f);
    TestEqual(TEXT("Health never goes negative"), FDamageCalculator::Apply(10.f, 999.f, 0.f), 0.f);
    return true;
}

#endif
