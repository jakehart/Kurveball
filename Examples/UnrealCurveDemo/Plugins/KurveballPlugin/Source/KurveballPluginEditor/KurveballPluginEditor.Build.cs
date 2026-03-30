using UnrealBuildTool;

public class KurveballPluginEditor : ModuleRules
{
	public KurveballPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("KurveballPluginEditor/Private");

		PublicDependencyModuleNames.AddRange
        (
			new string[]
			{
				"Core",
				"InputCore",
				"KurveballPluginRuntime"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange
        (
			new string[]
			{
                "Engine",
				"CoreUObject",
                "UnrealEd",
				"Slate",
				"SlateCore",
				"ToolMenus"
			}
		);
	}
}
