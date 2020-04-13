// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SandboxTerrainController.h"
#include "TerrainGenerator.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API ATerrainGenerator : public ASandboxTerrainController {
	GENERATED_BODY()
	
public:
		UFUNCTION(BlueprintCallable, Category = "UnrealSandbox")
		void StartTerrainGeneration();


protected:

	virtual void BeginPlayServer();

private:

	FCriticalSection GenerateTerrainMutex;
	bool bIsGenerationStarted;


};
