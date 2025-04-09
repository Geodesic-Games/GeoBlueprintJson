using UnrealBuildTool;

public class GeoBlueprintJson : ModuleRules
{
    public GeoBlueprintJson(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Json",
                "JsonUtilities",
                "BlueprintGraph"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "GraphEditor",
                "KismetCompiler",
                "UnrealEd"
            }
        );
    }
} 