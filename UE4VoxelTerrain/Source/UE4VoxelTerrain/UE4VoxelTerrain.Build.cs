// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE4VoxelTerrain : ModuleRules
{
	public UE4VoxelTerrain(ReadOnlyTargetRules Target) : base(Target)
    {
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UnrealSandboxTerrain", "UnrealSandboxToolkit" });
	}
}
