// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UE4VoxelTerrainEditorTarget : TargetRules
{
	public UE4VoxelTerrainEditorTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Editor;
        ExtraModuleNames.Add("UE4VoxelTerrain");
    }
}
