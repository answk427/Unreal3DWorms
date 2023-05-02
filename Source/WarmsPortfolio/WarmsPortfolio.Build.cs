// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WarmsPortfolio : ModuleRules
{
	public WarmsPortfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Niagara","CableComponent" });
	}
}
