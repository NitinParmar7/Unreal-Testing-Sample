# Unreal Testing Sample

Companion project for the blog post *Unit Testing in Unreal Engine*. It is a minimal C++ project (no content, no maps) containing:

| Path | What it shows |
|---|---|
| `Source/UnrealTestingSample/DamageCalculator.*` | Pure C++ logic, tested without a world |
| `Source/UnrealTestingSample/HealthComponent.*` | Component with timer-driven regeneration |
| `Source/UnrealTestingSampleTests/TestWorld.h` | RAII private `UWorld` you tick manually |
| `Source/UnrealTestingSampleTests/DamageCalculatorTest.cpp` | `IMPLEMENT_SIMPLE_AUTOMATION_TEST` |
| `Source/UnrealTestingSampleTests/HealthComponentSpec.cpp` | Automation Spec: damage, timers, latent test |

Targets Unreal Engine 5.8. For another 5.x version, update `EngineAssociation` in the `.uproject`, and use supported `DefaultBuildSettings` and `IncludeOrderVersion` values in both `*.Target.cs` files.

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

Expected: 13 passing tests (1 simple, 12 spec cases).
