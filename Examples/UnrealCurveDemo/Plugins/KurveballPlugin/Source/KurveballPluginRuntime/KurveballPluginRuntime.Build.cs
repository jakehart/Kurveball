using UnrealBuildTool;

public class KurveballPluginRuntime : ModuleRules
{
	public KurveballPluginRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("KurveballPluginRuntime/Private");

		PublicDependencyModuleNames.AddRange
        (
			new string[]
			{
				"Core",
				"InputCore"
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
