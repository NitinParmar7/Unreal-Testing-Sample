#include "Misc/AutomationTest.h"
#include "GasEffectBuilders.h"
#include "GasTestFixture.h"
#include "TestTime.h"
#include "TestWorld.h"

#if WITH_DEV_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(FGasEffectsSpec,
    "UnrealTestingSample.GAS.Effects",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)

    TUniquePtr<FTestWorld> TestWorld;
    GasTest::FFixture Fixture;

END_DEFINE_SPEC(FGasEffectsSpec)

void FGasEffectsSpec::Define()
{
    BeforeEach([this]()
    {
        TestWorld = MakeUnique<FTestWorld>();
        Fixture = GasTest::Spawn(TestWorld->Get());
    });

    AfterEach([this]()
    {
        Fixture = {};
        TestWorld.Reset();
    });

    Describe("Instant effects", [this]()
    {
        It("damage reduces health", [this]()
        {
            GasTest::Apply(Fixture.Asc, GasTest::MakeInstant(USampleAttributeSet::GetHealthAttribute(), -30.f));
            TestEqual("Health", GasTest::Get(Fixture, USampleAttributeSet::GetHealthAttribute()), 70.f);
        });

        It("health never drops below zero", [this]()
        {
            GasTest::Apply(Fixture.Asc, GasTest::MakeInstant(USampleAttributeSet::GetHealthAttribute(), -9999.f));
            TestEqual("Health", GasTest::Get(Fixture, USampleAttributeSet::GetHealthAttribute()), 0.f);
        });

        It("healing never exceeds max health", [this]()
        {
            GasTest::Apply(Fixture.Asc, GasTest::MakeInstant(USampleAttributeSet::GetHealthAttribute(), -20.f));
            GasTest::Apply(Fixture.Asc, GasTest::MakeInstant(USampleAttributeSet::GetHealthAttribute(), +500.f));
            TestEqual("Health", GasTest::Get(Fixture, USampleAttributeSet::GetHealthAttribute()), 100.f);
        });

        It("mana never goes negative", [this]()
        {
            GasTest::Apply(Fixture.Asc, GasTest::MakeInstant(USampleAttributeSet::GetManaAttribute(), -999.f));
            TestTrue("Mana is not negative", GasTest::Get(Fixture, USampleAttributeSet::GetManaAttribute()) >= 0.f);
        });
    });

    Describe("Duration effects", [this]()
    {
        LatentIt("a max health buff applies, then expires", [this](const FDoneDelegate& Done)
        {
            const FGameplayAttribute MaxHealth = USampleAttributeSet::GetMaxHealthAttribute();
            const FActiveGameplayEffectHandle Handle =
                GasTest::Apply(Fixture.Asc, GasTest::MakeTimed(MaxHealth, +50.f, 5.f));

            TestTrue("Effect is active", Handle.IsValid());
            TestEqual("Buffed max health", GasTest::Get(Fixture, MaxHealth), 150.f);

            AdvanceWorldFor(TestWorld.Get(), 4.9f, 0.1f, [this, Done, MaxHealth]()
            {
                TestEqual("Still buffed before expiry", GasTest::Get(Fixture, MaxHealth), 150.f);

                AdvanceWorldFor(TestWorld.Get(), 0.3f, 0.1f, [this, Done, MaxHealth]()
                {
                    TestEqual("Back to normal after expiry", GasTest::Get(Fixture, MaxHealth), 100.f);
                    Done.ExecuteIfBound();
                });
            });
        });

        It("removing the effect early restores the value immediately", [this]()
        {
            const FGameplayAttribute MaxHealth = USampleAttributeSet::GetMaxHealthAttribute();
            const FActiveGameplayEffectHandle Handle =
                GasTest::Apply(Fixture.Asc, GasTest::MakeTimed(MaxHealth, +50.f, 30.f));

            Fixture.Asc->RemoveActiveGameplayEffect(Handle);

            TestEqual("Value restored", GasTest::Get(Fixture, MaxHealth), 100.f);
        });
    });

    Describe("Periodic effects", [this]()
    {
        LatentIt("poison deals damage once per second", [this](const FDoneDelegate& Done)
        {
            const FGameplayAttribute Health = USampleAttributeSet::GetHealthAttribute();
            GasTest::Apply(Fixture.Asc, GasTest::MakePeriodic(Health, -5.f, 1.f, 5.f));

            TestEqual("No damage on application", GasTest::Get(Fixture, Health), 100.f);

            AdvanceWorldFor(TestWorld.Get(), 3.5f, 0.1f, [this, Done, Health]()
            {
                TestEqual("Three ticks of poison", GasTest::Get(Fixture, Health), 85.f);
                Done.ExecuteIfBound();
            });
        });

        LatentIt("poison stops when its duration ends", [this](const FDoneDelegate& Done)
        {
            const FGameplayAttribute Health = USampleAttributeSet::GetHealthAttribute();
            const FActiveGameplayEffectHandle Handle =
                GasTest::Apply(Fixture.Asc, GasTest::MakePeriodic(Health, -5.f, 1.f, 5.f));

            AdvanceWorldFor(TestWorld.Get(), 7.f, 0.1f, [this, Done, Health, Handle]()
            {
                const float HealthAfterExpiry = GasTest::Get(Fixture, Health);

                TestNull("Effect is gone", Fixture.Asc->GetActiveGameplayEffect(Handle));
                TestTrue("Poison dealt damage", HealthAfterExpiry < 100.f);

                AdvanceWorldFor(TestWorld.Get(), 3.f, 0.1f, [this, Done, Health, HealthAfterExpiry]()
                {
                    TestEqual("Health no longer changes", GasTest::Get(Fixture, Health), HealthAfterExpiry);
                    Done.ExecuteIfBound();
                });
            });
        });
    });
}

#endif
