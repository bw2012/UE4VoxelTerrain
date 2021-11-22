// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SandboxTerrainController.h"
#include "LevelController.h"
#include "TerrainController.generated.h"


struct TVdGenBlock;

typedef int(*PCudaGetInfo)();
typedef int(*PCudaGenerateVd)(TVdGenBlock*);

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API ATerrainController : public ASandboxTerrainController
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Debug")
	bool bUseCUDAGenerator = false;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	ALevelController* LevelController;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Progress Save Voxel Terrain"))
	void OnProgressSaveTerrain(float Progress);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Finish Save Voxel Terrain"))
	void OnFinishSaveTerrain();

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox")
	void ShutdownAndSaveMap();


public:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	bool IsUseCuda();

	void* CudaGenVdPtr = nullptr; //FIXME

protected:

	virtual UTerrainGeneratorComponent* NewTerrainGenerator() override;

	virtual void OnOverlapActorDuringTerrainEdit(const FHitResult& OverlapResult, const FVector& Pos) override;

private:

	void* CudaGenDllHandle = nullptr;

};
