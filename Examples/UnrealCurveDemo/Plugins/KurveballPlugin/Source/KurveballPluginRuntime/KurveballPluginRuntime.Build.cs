using UnrealBuildTool;

public class KurveballPluginRuntime : ModuleRules
{
	public KurveballPluginRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add("KurveballPluginRuntime/Public");
		PublicIncludePaths.Add("KurveballPluginRuntime/Public/UnrealWrapper");

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
				"UMG",
				"Slate",
				"SlateCore",
				"ToolMenus"
			}
		);
	}
}
