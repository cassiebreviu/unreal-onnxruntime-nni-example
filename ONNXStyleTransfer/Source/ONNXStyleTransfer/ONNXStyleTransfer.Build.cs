// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ONNXStyleTransfer : ModuleRules
{
	public ONNXStyleTransfer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		//added to supress warnings from opencv plugin
		bEnableUndefinedIdentifierWarnings = false;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "OpenCV", "OpenCVHelper", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "OpenCV","OpenCVHelper"});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
