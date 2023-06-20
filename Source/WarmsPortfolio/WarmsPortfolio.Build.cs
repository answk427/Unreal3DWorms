// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;



public class WarmsPortfolio : ModuleRules
{
	public WarmsPortfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        string enginePath = System.IO.Path.GetFullPath(Target.RelativeEnginePath);
        string voxelPluginPath = enginePath + ("Plugins/Marketplace/VoxelFree/Source");

        
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Niagara","CableComponent", "Voxel", "CinematicCamera"});
        PublicIncludePaths.AddRange
        (
            new string[]
            {
                "WarmsPortfolio",
                voxelPluginPath
            }
        );

        PrivateIncludePaths.AddRange
        (
            new string[]
            {
                "WarmsPortfolio",
                voxelPluginPath
            }
        );
    }
}
