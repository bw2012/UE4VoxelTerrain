// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SandboxObject.h"
#include "BaseCharacter.h"
#include "SandboxTerrainController.h"
#include "MiningTool.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API AMiningTool : public ASandboxObject
{
	GENERATED_BODY()

public:

	AMiningTool();

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	USoundCue* DiggingRockSound;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	USoundCue* DiggingSoilSound;

	void OnAltAction(const FHitResult& Hit, ABaseCharacter* Owner);

	void OnTracePlayerActionPoint(const FHitResult& Res, ABaseCharacter* Owner);

	void ToggleToolMode() { DiggingToolMode++; DiggingToolMode = DiggingToolMode % 4; };

private:

	int DiggingToolMode = 0;

	void PlaySound(const FSandboxTerrainMaterial& MatInfo, const FVector& Location, UWorld* World);

};
