// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UE4VoxelTerrainTarget : TargetRules
{
	public UE4VoxelTerrainTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Game;
        ExtraModuleNames.Add("UE4VoxelTerrain");
    }
}
