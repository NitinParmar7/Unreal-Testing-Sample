using UnrealBuildTool;

public class UnrealTestingSample : ModuleRules
{
    public UnrealTestingSample(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(ModuleDirectory);
        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core", "CoreUObject", "Engine",
            "GameplayAbilities", "GameplayTags", "GameplayTasks"
        });
    }
}
