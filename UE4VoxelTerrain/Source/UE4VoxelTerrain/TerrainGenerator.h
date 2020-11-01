// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TerrainController.h"
#include "TerrainGenerator.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API ATerrainGenerator : public ATerrainController {
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox")
	void StartBuildingTerrain();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Finish Build Sandbox Terrain"))
	void OnFinishBuildingTerrain();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Progress Build Sandbox Terrain"))
	void OnProgressBuildingTerrain(float Progress);

protected:

	virtual void BeginTerrainLoad() override;

private:

	void FinishGenerationPipeline();

	void ProgressGenerationPipeline(uint32 Progress, uint32 Total);

	FCriticalSection GenerateTerrainMutex;
	bool bIsGenerationStarted;
};
