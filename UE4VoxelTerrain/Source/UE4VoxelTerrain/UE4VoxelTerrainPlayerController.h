// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "SandboxPlayerController.h"
#include "SandboxEnvironment.h"
#include "UE4VoxelTerrainPlayerController.generated.h"


UCLASS()
class AUE4VoxelTerrainPlayerController : public ASandboxPlayerController
{
	GENERATED_BODY()

public:
	AUE4VoxelTerrainPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void ToggleConstructionMode() { bIsConstructionMode = !bIsConstructionMode;  };

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void ToggleToolMode() { DiggingToolMode++; DiggingToolMode = DiggingToolMode % 2; };

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

public:

	int DiggingToolMode = 0;

private:

	ASandboxEnvironment* SandboxEnvironment;

	bool bIsConstructionMode;

	FTimerHandle Timer;

	ASandboxObject* GetCurrentInventoryObject();

	FVector PrevLocation;
};


