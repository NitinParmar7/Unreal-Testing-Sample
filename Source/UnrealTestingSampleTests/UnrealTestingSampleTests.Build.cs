using UnrealBuildTool;

public class UnrealTestingSampleTests : ModuleRules
{
    public UnrealTestingSampleTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UnrealTestingSample" });
    }
}
