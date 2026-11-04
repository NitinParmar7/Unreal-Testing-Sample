#include "Misc/AutomationTest.h"
#include "Abilities/GameplayAbility.h"
#include "FireballAbility.h"
#include "GasTestFixture.h"
#include "SampleGameplayTags.h"
#include "TestTime.h"
#include "TestWorld.h"

#if WITH_DEV_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(FGasAbilitySpec,
    "UnrealTestingSample.GAS.Abilities.Fireball",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)

    TUniquePtr<FTestWorld> TestWorld;
    GasTest::FFixture Fixture;
    FGameplayAbilitySpecHandle Fireball;

    float Mana() const { return GasTest::Get(Fixture, USampleAttributeSet::GetManaAttribute()); }

END_DEFINE_SPEC(FGasAbilitySpec)

void FGasAbilitySpec::Define()
{
    BeforeEach([this]()
    {
        TestWorld = MakeUnique<FTestWorld>();
        Fixture = GasTest::Spawn(TestWorld->Get(), /*MaxHealth*/ 100.f, /*Mana*/ 50.f);

        Fireball = Fixture.Asc->GiveAbility(
            FGameplayAbilitySpec(UFireballAbility::StaticClass(), /*Level*/ 1, INDEX_NONE, Fixture.Actor));
        TestTrue("Ability granted", Fireball.IsValid());
    });

    AfterEach([this]()
    {
        Fireball = FGameplayAbilitySpecHandle();
        Fixture = {};
        TestWorld.Reset();
    });

    Describe("Activation and cost", [this]()
    {
        It("activates and spends 25 mana", [this]()
        {
            TestTrue("Activated", Fixture.Asc->TryActivateAbility(Fireball));
            TestEqual("Mana after cast", Mana(), 25.f);
        });

        It("does not activate without enough mana", [this]()
        {
            Fixture.Asc->SetNumericAttributeBase(USampleAttributeSet::GetManaAttribute(), 10.f);

            TestFalse("Activation refused", Fixture.Asc->TryActivateAbility(Fireball));
            TestEqual("Mana untouched", Mana(), 10.f);
        });

        It("puts the ability on cooldown", [this]()
        {
            Fixture.Asc->TryActivateAbility(Fireball);
            TestTrue("Cooldown tag present",
                Fixture.Asc->HasMatchingGameplayTag(SampleTags::Cooldown_Fireball.GetTag()));
        });
    });

    Describe("Gating", [this]()
    {
        It("cannot be recast during the cooldown", [this]()
        {
            TestTrue("First cast", Fixture.Asc->TryActivateAbility(Fireball));
            TestFalse("Second cast refused", Fixture.Asc->TryActivateAbility(Fireball));
            TestEqual("Only one cost paid", Mana(), 25.f);
        });

        It("cannot be cast while stunned", [this]()
        {
            Fixture.Asc->AddLooseGameplayTag(SampleTags::State_Stunned.GetTag());

            TestFalse("Activation refused", Fixture.Asc->TryActivateAbility(Fireball));
            TestEqual("Mana untouched", Mana(), 50.f);
        });

        It("can be cast again once the stun is removed", [this]()
        {
            Fixture.Asc->AddLooseGameplayTag(SampleTags::State_Stunned.GetTag());
            Fixture.Asc->RemoveLooseGameplayTag(SampleTags::State_Stunned.GetTag());

            TestTrue("Activated", Fixture.Asc->TryActivateAbility(Fireball));
        });
    });

    Describe("Cooldown timing", [this]()
    {
        LatentIt("is still blocked just before 3 seconds", [this](const FDoneDelegate& Done)
        {
            Fixture.Asc->TryActivateAbility(Fireball);

            AdvanceWorldFor(TestWorld.Get(), 2.9f, 0.1f, [this, Done]()
            {
                TestFalse("Still on cooldown", Fixture.Asc->TryActivateAbility(Fireball));
                Done.ExecuteIfBound();
            });
        });

        LatentIt("can be cast again after 3 seconds, until mana runs out", [this](const FDoneDelegate& Done)
        {
            Fixture.Asc->TryActivateAbility(Fireball);                 // mana 50 -> 25

            AdvanceWorldFor(TestWorld.Get(), 3.2f, 0.1f, [this, Done]()
            {
                TestFalse("Cooldown tag gone",
                    Fixture.Asc->HasMatchingGameplayTag(SampleTags::Cooldown_Fireball.GetTag()));
                TestTrue("Second cast works", Fixture.Asc->TryActivateAbility(Fireball));   // mana 25 -> 0
                TestEqual("Mana spent", Mana(), 0.f);

                AdvanceWorldFor(TestWorld.Get(), 3.2f, 0.1f, [this, Done]()
                {
                    TestFalse("Third cast refused: no mana", Fixture.Asc->TryActivateAbility(Fireball));
                    Done.ExecuteIfBound();
                });
            });
        });
    });
}

#endif
