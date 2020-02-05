// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class LoadingScreen : ModuleRules
{
	public LoadingScreen(ReadOnlyTargetRules Target) : base(Target)
	{
        PublicIncludePaths.AddRange(new string[] { "LoadingScreen/Public" });

        PrivateIncludePaths.AddRange(new string[] { "LoadingScreen/Private" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "InputCore", "Project_Ombres" });
		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "MoviePlayer" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
