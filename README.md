# Unreal Testing Sample

Companion project for the blog posts *Unit Testing in Unreal Engine* and *Testing the Gameplay Ability System in Unreal Engine*. It is a minimal C++ project (no content, no maps) containing:

| Path | What it shows |
|---|---|
| `Source/UnrealTestingSample/DamageCalculator.*` | Pure C++ logic, tested without a world |
| `Source/UnrealTestingSample/HealthComponent.*` | Component with timer-driven regeneration |
| `Source/UnrealTestingSampleTests/TestWorld.h` | RAII private `UWorld` you tick manually |
| `Source/UnrealTestingSampleTests/DamageCalculatorTest.cpp` | `IMPLEMENT_SIMPLE_AUTOMATION_TEST` |
| `Source/UnrealTestingSampleTests/HealthComponentSpec.cpp` | Automation Spec: damage, timers, latent test |
| `Source/UnrealTestingSampleTests/TestTime.h` | `AdvanceWorldFor`: simulate time one frame at a time |
| `Source/UnrealTestingSample/SampleAttributeSet.*` | GAS attribute set (Health, MaxHealth, Mana) with clamping |
| `Source/UnrealTestingSample/FireballAbility.*`, `FireballEffects.*` | GAS ability with mana cost, 3 s cooldown, stun blocking |
| `Source/UnrealTestingSample/SampleGameplayTags.*` | Native gameplay tags |
| `Source/UnrealTestingSampleTests/GasTestFixture.h`, `GasEffectBuilders.h` | Actor + ASC fixture and runtime effect builders |
| `Source/UnrealTestingSampleTests/GasEffectsSpec.cpp` | Instant, duration, and periodic effect tests |
| `Source/UnrealTestingSampleTests/GasAbilitySpec.cpp` | Activation, cost, cooldown, and gating tests |

Targets Unreal Engine 5.8. Both `*.Target.cs` files use `DefaultBuildSettings = BuildSettingsVersion.V7` and `IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8`. For another 5.x version, update `EngineAssociation` in the `.uproject` and use the supported settings for that engine version.

## Build

1. Right-click `UnrealTestingSample.uproject` → *Generate Visual Studio project files* (or `GenerateProjectFiles` on Mac/Linux).
2. Build the `UnrealTestingSampleEditor` target (Development Editor), or just open the `.uproject` and accept the rebuild prompt.

## Run the tests in the editor

*Tools → Session Frontend → Automation* tab, expand `UnrealTestingSample`, tick it, *Start Tests*.

## Run headless (CI style)

```bash
UnrealEditor-Cmd UnrealTestingSample.uproject \
  -ExecCmds="Automation RunTests UnrealTestingSample; Quit" \
  -unattended -nopause -nosplash -NullRHI \
  -ReportExportPath="Saved/AutomationReports" -log
```

Expected: 30 passing tests (1 simple, 13 health component spec cases, 8 GAS effect cases, 8 GAS ability cases).

Run only the GAS tests with `Automation RunTests UnrealTestingSample.GAS`.
