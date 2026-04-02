using UnrealBuildTool;

public class KurveballPluginEditor : ModuleRules
{
	public KurveballPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange
        (
			new string[]
			{
				"Core",
				"InputCore",
                "UnrealEd",
				"EditorFramework",
				"AssetDefinition",
				"KurveballPluginRuntime"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange
        (
			new string[]
			{
                "Engine",
				"CoreUObject",
				"Slate",
				"SlateCore",
				"ToolMenus"
			}
		);
	}
}
