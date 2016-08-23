// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SandboxTerrainController.h"
#include "TerrainController.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API ATerrainController : public ASandboxTerrainController
{
	GENERATED_BODY()
	
	
public:
	virtual SandboxVoxelGenerator newTerrainGenerator(VoxelData &voxel_data) override;
	
};
