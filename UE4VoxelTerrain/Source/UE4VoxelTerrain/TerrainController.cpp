// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "TerrainController.h"


SandboxVoxelGenerator ATerrainController::newTerrainGenerator(VoxelData &voxel_data) {
	UE_LOG(LogTemp, Warning, TEXT("terrain generator override"));
	return ASandboxTerrainController::newTerrainGenerator(voxel_data);
}

