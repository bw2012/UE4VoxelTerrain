// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainCharacter.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "SandboxTerrainController.h"

AUE4VoxelTerrainCharacter::AUE4VoxelTerrainCharacter() {


}

void AUE4VoxelTerrainCharacter::Tick(float DeltaSeconds) {

	if (GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	//if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_WorldStatic, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			//CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			//CursorToWorld->SetWorldRotation(CursorR);

			if (!TraceHitResult.bBlockingHit) {
				return;
			}

			ASandboxTerrainController* TerrainController = Cast<ASandboxTerrainController>(TraceHitResult.Actor.Get());

			if (TerrainController != nullptr) {
				AUE4VoxelTerrainPlayerController* controller = Cast<AUE4VoxelTerrainPlayerController>(GetController());

				if (controller->tool_mode == 1) {
					DrawDebugSphere(GetWorld(), TraceHitResult.Location, 80, 24, FColor(255, 255, 255, 100));
				}

				if (controller->tool_mode == 2) {
					DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(105), FColor(255, 255, 255, 100));
				}
			}
		}
	}
}
