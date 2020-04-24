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

	int32 ZoneLoaderConter = 0;
	int32 ZoneLoaderTotal = 0;

protected:

	virtual float GeneratorDensityFunc(const TVoxelDensityFunctionData& FunctionData);

	virtual bool GeneratorForcePerformZone(const TVoxelIndex& ZoneIndex) override;

};
