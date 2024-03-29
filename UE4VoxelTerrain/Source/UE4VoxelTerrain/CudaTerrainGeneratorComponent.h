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

	virtual float DensityFunctionExt(float Density, const TVoxelIndex& ZoneIndex, const FVector& WorldPos, const FVector& LocalPos) const override;

	virtual FSandboxFoliage FoliageExt(const int32 FoliageTypeId, const FSandboxFoliage & FoliageType, const TVoxelIndex & ZoneIndex, const FVector & WorldPos) override;

protected:

	virtual void BatchGenerateComplexVd(TArray<TGenerateVdTempItm>& GenPass2List) override;

	virtual void OnBatchGenerationFinished() override;

	virtual bool IsForcedComplexZone(const TVoxelIndex& ZoneIndex) override;

	void virtual PrepareMetaData() override;


public:

	float FunctionMakeBox(const float InDensity, const FVector& P, const FBox& InBox) const;

	float FunctionMakeCaveLayer(float Density, const FVector& WorldPos) const;

	float FunctionMakeVerticalCylinder(const float InDensity, const FVector& V, const FVector& Origin, const float Radius, const float Top, const float Bottom) const;

};
