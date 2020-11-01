// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "SandboxPlayerController.h"
#include "SandboxEnvironment.h"
#include "LevelController.h"
#include "TerrainController.h"
#include "UE4VoxelTerrainPlayerController.generated.h"


UCLASS()
class AUE4VoxelTerrainPlayerController : public ASandboxPlayerController
{
	GENERATED_BODY()

public:
	AUE4VoxelTerrainPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void ToggleToolMode();

	virtual void BeginPlay() override;

protected:

	virtual void PlayerTick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	virtual void OnMainActionPressed();

	virtual void OnMainActionReleased();

	virtual void OnAltActionPressed();

	virtual void OnAltActionReleased();

	void PerformAction();

	virtual void OnTracePlayerActionPoint(const FHitResult& Res) override;


private:

	UPROPERTY()
	ASandboxEnvironment* SandboxEnvironment;

	UPROPERTY()
	ALevelController* LevelController;

	UPROPERTY()
	ATerrainController* TerrainController;


	//bool bIsConstructionMode;
	bool bPlaceCurrentObjectToWorld;

	FTimerHandle Timer;

	ASandboxObject* GetCurrentInventoryObject();

	FVector PrevLocation;
};


