using UnrealBuildTool;

public class CurvePluginEditor : ModuleRules
{
	public CurvePluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("CurvePluginEditor/Private");

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
                "UnrealEd"
			}
		);
	}
}
