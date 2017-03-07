// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "TerrainController.h"


SandboxVoxelGenerator ATerrainController::newTerrainGenerator(TVoxelData &voxel_data) {
	//UE_LOG(LogTemp, Warning, TEXT("terrain generator override"));
	return ASandboxTerrainController::newTerrainGenerator(voxel_data);
}

void ATerrainController::OnLoadZoneProgress(int progress, int total) {
	//UE_LOG(LogTemp, Warning, TEXT("%d / %d"), progress, total);

	ZoneLoaderConter = progress;
	ZoneLoaderTotal = total;
}

void ATerrainController::OnLoadZoneListFinished() {
	ZoneLoaderConter = 0;
	ZoneLoaderTotal = 0;
}