
using UnrealBuildTool;

public class Project_Ombres : ModuleRules
{
	public Project_Ombres(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" , "MoviePlayer" ,"OnlineSubsystem","OnlineSubsystemUtils","Steamworks"});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore","OnlineSubsystem" });
	}
}
