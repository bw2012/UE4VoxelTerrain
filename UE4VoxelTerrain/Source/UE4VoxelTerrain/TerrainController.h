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
	virtual SandboxVoxelGenerator newTerrainGenerator(TVoxelData &voxel_data) override;

	int32 ZoneLoaderConter = 0;
	int32 ZoneLoaderTotal = 0;

protected:

	virtual void OnLoadZoneProgress(int progress, int total) override;

	virtual void OnLoadZoneListFinished() override;
	
};
