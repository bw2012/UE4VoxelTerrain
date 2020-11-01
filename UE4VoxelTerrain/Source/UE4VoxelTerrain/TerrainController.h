// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SandboxTerrainController.h"
#include "LevelController.h"
#include "TerrainController.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API ATerrainController : public ASandboxTerrainController
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	ALevelController* LevelController;

protected:

	virtual float GeneratorDensityFunc(const TVoxelDensityFunctionData& FunctionData);

	virtual bool GeneratorForcePerformZone(const TVoxelIndex& ZoneIndex) override;

	virtual void OnOverlapActorDuringTerrainEdit(const FHitResult& OverlapResult, const FVector& Pos) override;

	virtual FSandboxFoliage GeneratorFoliageOverride(const int32 FoliageTypeId, const FSandboxFoliage& FoliageType, const TVoxelIndex& ZoneIndex, const FVector& WorldPos) override;

	virtual bool GeneratorUseCustomFoliage(const TVoxelIndex& Index);

	virtual bool GeneratorSpawnCustomFoliage(const TVoxelIndex& Index, const FVector& WorldPos, int32 FoliageTypeId, FSandboxFoliage FoliageType, FRandomStream& Rnd, FTransform& Transform);
};
