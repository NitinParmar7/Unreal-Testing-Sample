using UnrealBuildTool;

public class UnrealTestingSampleEditorTarget : TargetRules
{
    public UnrealTestingSampleEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.AddRange(new[] { "UnrealTestingSample", "UnrealTestingSampleTests" });
    }
}
