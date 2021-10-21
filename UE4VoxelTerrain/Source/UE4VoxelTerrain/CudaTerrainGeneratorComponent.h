// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TerrainGeneratorComponent.h"
#include "CudaTerrainGeneratorComponent.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API UCudaTerrainGeneratorComponent : public UTerrainGeneratorComponent {
	GENERATED_BODY()
	
public:

	void BeginPlay() override;

	virtual bool ForcePerformZone(const TVoxelIndex& ZoneIndex) override;

	virtual float DensityFunctionExt(float Density, const TVoxelIndex& ZoneIndex, const FVector& WorldPos, const FVector& LocalPos) const override;

	virtual bool UseCustomFoliage(const TVoxelIndex& ZoneIndex) override;

	virtual FSandboxFoliage FoliageExt(const int32 FoliageTypeId, const FSandboxFoliage & FoliageType, const TVoxelIndex & ZoneIndex, const FVector & WorldPos) override;

	virtual bool SpawnCustomFoliage(const TVoxelIndex& Index, const FVector& WorldPos, int32 FoliageTypeId, FSandboxFoliage FoliageType, FRandomStream& Rnd, FTransform& Transform) override;

protected:

	virtual void BatchGenerateComplexVd(TArray<TGenerateVdTempItm>& GenPass2List) override;

	virtual void OnBatchGenerationFinished() override;

};
