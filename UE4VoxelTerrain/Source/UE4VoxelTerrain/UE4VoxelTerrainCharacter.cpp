// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainCharacter.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "SandboxTerrainController.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "VoxelDualContouringMeshComponent.h"

#include <cmath>

AUE4VoxelTerrainCharacter::AUE4VoxelTerrainCharacter() {


}

void AUE4VoxelTerrainCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	//if (CursorToWorld != nullptr)
	{
		if (AUE4VoxelTerrainPlayerController* PC = Cast<AUE4VoxelTerrainPlayerController>(GetController())) {
			FHitResult TraceHitResult = PC->TracePlayerActionPoint();

			if (!TraceHitResult.bBlockingHit) {
				return;
			}

			ASandboxTerrainController* TerrainController = Cast<ASandboxTerrainController>(TraceHitResult.Actor.Get());

			UVoxelDualContouringMeshComponent* DualContouringVoxelMesh = Cast<UVoxelDualContouringMeshComponent>(TraceHitResult.Component.Get());
			if (DualContouringVoxelMesh != nullptr) {
				AUE4VoxelTerrainPlayerController* controller = Cast<AUE4VoxelTerrainPlayerController>(GetController());
				if (controller->tool_mode == 1) {
					DrawDebugSphere(GetWorld(), TraceHitResult.Location, 80, 24, FColor(255, 255, 255, 100));
				}
			}

			if (TerrainController != nullptr) {
				AUE4VoxelTerrainPlayerController* controller = Cast<AUE4VoxelTerrainPlayerController>(GetController());

				if (controller->tool_mode == 1) {
					DrawDebugSphere(GetWorld(), TraceHitResult.Location, 80, 24, FColor(255, 255, 255, 100));
				}

				if (controller->tool_mode == 2) {
					DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(105), FColor(255, 255, 255, 100));
				}

				if (controller->tool_mode == 3) {
					static const float GridRange = 100;
					FVector Tmp(TraceHitResult.Location);
					Tmp /= GridRange;
					Tmp.Set(std::round(Tmp.X), std::round(Tmp.Y), std::round(Tmp.Z));
					Tmp *= GridRange;
					FVector Position((int)Tmp.X, (int)Tmp.Y, (int)Tmp.Z);

					DrawDebugBox(GetWorld(), Position, FVector(100), FColor(255, 255, 255, 100));
				}

				if (controller->tool_mode == 4) {
					DrawDebugSphere(GetWorld(), TraceHitResult.Location, 60, 24, FColor(100, 255, 255, 100));
				}

				if (controller->tool_mode == 5) {
					DrawDebugSphere(GetWorld(), TraceHitResult.Location, 60, 24, FColor(255, 255, 100, 100));
				}

				if (controller->tool_mode == 6) {
					DrawDebugSphere(GetWorld(), TraceHitResult.Location, 60, 24, FColor(255, 100, 255, 100));
				}

				if (controller->tool_mode == 7) {
					static const float GridRange = 100;
					FVector Tmp(TraceHitResult.Location);
					Tmp /= GridRange;
					Tmp.Set(std::round(Tmp.X), std::round(Tmp.Y), std::floor(Tmp.Z));
					Tmp *= GridRange;
					FVector Position((int)Tmp.X, (int)Tmp.Y, ((int)Tmp.Z) + GridRange);

					DrawDebugBox(GetWorld(), Position, FVector(50), FColor(100, 100, 100, 100));
				}
			}
		}
	}
}

