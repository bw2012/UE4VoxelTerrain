// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SandboxTerrainController.h"
#include "SandboxCharacter.h"

AUE4VoxelTerrainPlayerController::AUE4VoxelTerrainPlayerController() {
	tool_mode = 1;
}

void AUE4VoxelTerrainPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);
}

void AUE4VoxelTerrainPlayerController::SetupInputComponent() {
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("1", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool1);
	InputComponent->BindAction("2", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool2);
}

void AUE4VoxelTerrainPlayerController::OnMainActionPressed() {
	SetDestinationPressed();
}

void AUE4VoxelTerrainPlayerController::OnMainActionReleased() {
	SetDestinationReleased();
}

void AUE4VoxelTerrainPlayerController::OnAltActionPressed() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	if (Hit.bBlockingHit) {
		UE_LOG(LogTemp, Warning, TEXT("test point -> %f %f %f"), Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z);
		ASandboxTerrainController* terrain = Cast<ASandboxTerrainController>(Hit.Actor.Get());
		if (terrain != NULL) {
			if (tool_mode == 1) {
				terrain->digTerrainRoundHole(Hit.ImpactPoint, 80, 5);
				GetWorld()->GetTimerManager().SetTimer(timer, this, &AUE4VoxelTerrainPlayerController::PerformAction, 0.2, true);
			}

			if (tool_mode == 2) {
				terrain->digTerrainCubeHole(Hit.ImpactPoint, 110, 5);
			}
		}
	}
}

void AUE4VoxelTerrainPlayerController::OnAltActionReleased() {
	GetWorld()->GetTimerManager().ClearTimer(timer);
}

void AUE4VoxelTerrainPlayerController::setTool1() {
	tool_mode = 1;
}

void AUE4VoxelTerrainPlayerController::setTool2() {
	tool_mode = 2;
}

void AUE4VoxelTerrainPlayerController::PerformAction() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	if (Hit.bBlockingHit) {
		ASandboxTerrainController* terrain = Cast<ASandboxTerrainController>(Hit.Actor.Get());
		if (terrain != NULL) {
			if (tool_mode == 1) {
				terrain->digTerrainRoundHole(Hit.ImpactPoint, 80, 5);
			}
		}
	}
}


