#include "Misc/AutomationTest.h"
#include "Async/Async.h"
#include "Containers/Ticker.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.h"
#include "TestWorld.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    UHealthComponent* SpawnHealthComponent(UWorld* World, AActor*& OutActor, float MaxHealth)
    {
        OutActor = World->SpawnActorDeferred<AActor>(AActor::StaticClass(), FTransform::Identity);
        UHealthComponent* Health = NewObject<UHealthComponent>(OutActor);
        Health->MaxHealth = MaxHealth;
        Health->RegenDelay = 5.f;
        Health->RegenPerSecond = 10.f;
        OutActor->AddInstanceComponent(Health);
        Health->RegisterComponent();
        OutActor->FinishSpawning(FTransform::Identity);
        OutActor->DispatchBeginPlay();
        return Health;
    }

    void AdvanceWorldFor(FTestWorld* TestWorld, float Duration, float FrameDelta, TFunction<void()> OnComplete)
    {
        const int32 FrameCount = FMath::CeilToInt(Duration / FrameDelta);
        FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
            [TestWorld, FramesRemaining = FrameCount, FrameDelta, bPrimed = false, OnComplete = MoveTemp(OnComplete)](float) mutable
            {
                if (!bPrimed)
                {
                    TestWorld->Tick(0.f);
                    bPrimed = true;
                    return true;
                }

                TestWorld->Tick(FrameDelta);
                if (--FramesRemaining == 0)
                {
                    OnComplete();
                    return false;
                }

                return true;
            }));
    }
}

BEGIN_DEFINE_SPEC(FHealthComponentSpec,
    "UnrealTestingSample.Gameplay.HealthComponent",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)

    TUniquePtr<FTestWorld> TestWorld;
    AActor* Actor = nullptr;
    UHealthComponent* Health = nullptr;
    bool bDied = false;

END_DEFINE_SPEC(FHealthComponentSpec)

void FHealthComponentSpec::Define()
{
    BeforeEach([this]()
    {
        bDied = false;
        TestWorld = MakeUnique<FTestWorld>();

        Health = SpawnHealthComponent(TestWorld->Get(), Actor, 100.f);
        TestTrue("Component began play", Health->HasBegunPlay());
    });

    AfterEach([this]()
    {
        Health = nullptr;
        Actor = nullptr;
        TestWorld.Reset();
    });

    Describe("Taking damage", [this]()
    {
        It("starts at max health", [this]()
        {
            TestEqual("Initial health", Health->GetHealth(), 100.f);
        });

        It("initializes to its configured maximum health", [this]()
        {
            AActor* ConfiguredActor = nullptr;
            UHealthComponent* ConfiguredHealth = SpawnHealthComponent(TestWorld->Get(), ConfiguredActor, 175.f);
            TestTrue("Configured health component began play", ConfiguredHealth->HasBegunPlay());
            TestEqual("Initial health uses configured maximum", ConfiguredHealth->GetHealth(), 175.f);
        });

        It("reduces health by the damage amount", [this]()
        {
            Health->TakeDamage(30.f);
            TestEqual("Health after hit", Health->GetHealth(), 70.f);
        });

        It("never drops below zero", [this]()
        {
            Health->TakeDamage(9999.f);
            TestEqual("Clamped health", Health->GetHealth(), 0.f);
            TestTrue("Is dead", Health->IsDead());
        });

        It("ignores damage once dead", [this]()
        {
            Health->TakeDamage(100.f);
            Health->TakeDamage(50.f);
            TestEqual("Still zero", Health->GetHealth(), 0.f);
        });
    });

    Describe("Regeneration (timers)", [this]()
    {
        LatentIt("does not regenerate before the delay elapses", [this](const FDoneDelegate& Done)
        {
            Health->TakeDamage(50.f);
            AdvanceWorldFor(TestWorld.Get(), 4.9f, 0.1f, [this, Done]()
            {
                TestEqual("Health unchanged inside delay", Health->GetHealth(), 50.f);
                Done.ExecuteIfBound();
            });
        });

        LatentIt("starts regenerating after the delay", [this](const FDoneDelegate& Done)
        {
            Health->TakeDamage(50.f);
            AdvanceWorldFor(TestWorld.Get(), 5.1f, 0.1f, [this, Done]()
            {
                TestEqual("No regen before the first regen interval", Health->GetHealth(), 50.f);
                AdvanceWorldFor(TestWorld.Get(), 1.1f, 0.1f, [this, Done]()
                {
                    TestEqual("One regen tick", Health->GetHealth(), 60.f);
                    Done.ExecuteIfBound();
                });
            });
        });

        LatentIt("regenerates at the configured rate", [this](const FDoneDelegate& Done)
        {
            Health->TakeDamage(50.f);
            AdvanceWorldFor(TestWorld.Get(), 5.1f, 0.1f, [this, Done]()
            {
                AdvanceWorldFor(TestWorld.Get(), 3.3f, 0.1f, [this, Done]()
                {
                    TestEqual("Three regen ticks", Health->GetHealth(), 80.f);
                    Done.ExecuteIfBound();
                });
            });
        });

        LatentIt("stops at max health", [this](const FDoneDelegate& Done)
        {
            Health->TakeDamage(25.f);
            AdvanceWorldFor(TestWorld.Get(), 5.1f, 0.1f, [this, Done]()
            {
                AdvanceWorldFor(TestWorld.Get(), 60.f, 1.f, [this, Done]()
                {
                    TestEqual("Clamped to max", Health->GetHealth(), 100.f);
                    Done.ExecuteIfBound();
                });
            });
        });

        LatentIt("restarts the delay when damaged again", [this](const FDoneDelegate& Done)
        {
            Health->TakeDamage(50.f);
            AdvanceWorldFor(TestWorld.Get(), 4.f, 0.1f, [this, Done]()
            {
                Health->TakeDamage(10.f);
                AdvanceWorldFor(TestWorld.Get(), 4.9f, 0.1f, [this, Done]()
                {
                    TestEqual("No regen yet", Health->GetHealth(), 40.f);
                    AdvanceWorldFor(TestWorld.Get(), 0.2f, 0.1f, [this, Done]()
                    {
                        AdvanceWorldFor(TestWorld.Get(), 1.2f, 0.1f, [this, Done]()
                        {
                            TestEqual("Regen resumed", Health->GetHealth(), 50.f);
                            Done.ExecuteIfBound();
                        });
                    });
                });
            });
        });

        LatentIt("regenerates at 60 FPS within one regen step of the big-step result", [this](const FDoneDelegate& Done)
        {
            Health->TakeDamage(50.f);
            AdvanceWorldFor(TestWorld.Get(), 8.1f, 1.f / 60.f, [this, Done]()
            {
                TestEqual("Delay (5s) + 3 regen ticks", Health->GetHealth(), 80.f, 10.f);
                Done.ExecuteIfBound();
            });
        });

        LatentIt("does not regenerate after death", [this](const FDoneDelegate& Done)
        {
            Health->TakeDamage(100.f);
            AdvanceWorldFor(TestWorld.Get(), 20.f, 0.1f, [this, Done]()
            {
                TestEqual("Stays dead", Health->GetHealth(), 0.f);
                Done.ExecuteIfBound();
            });
        });
    });

    Describe("Events", [this]()
    {
        LatentIt("fires OnDied when health reaches zero", [this](const FDoneDelegate& Done)
        {
            Health->OnDied.AddLambda([this]() { bDied = true; });
            Health->TakeDamage(1000.f);

            AsyncTask(ENamedThreads::GameThread, [this, Done]()
            {
                TestTrue("OnDied fired", bDied);
                Done.ExecuteIfBound();
            });
        });
    });
}

#endif