// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CurveDemo : ModuleRules
{
	public CurveDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"CurveDemo",
			"CurveDemo/Variant_Platforming",
			"CurveDemo/Variant_Platforming/Animation",
			"CurveDemo/Variant_Combat",
			"CurveDemo/Variant_Combat/AI",
			"CurveDemo/Variant_Combat/Animation",
			"CurveDemo/Variant_Combat/Gameplay",
			"CurveDemo/Variant_Combat/Interfaces",
			"CurveDemo/Variant_Combat/UI",
			"CurveDemo/Variant_SideScrolling",
			"CurveDemo/Variant_SideScrolling/AI",
			"CurveDemo/Variant_SideScrolling/Gameplay",
			"CurveDemo/Variant_SideScrolling/Interfaces",
			"CurveDemo/Variant_SideScrolling/UI"
		});

		PublicDefinitions.Add("NUGET_WARNING_AS_ERROR_DISABLED=NU1510");
		
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
