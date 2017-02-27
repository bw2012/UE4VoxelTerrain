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

	InputComponent->BindAction("0", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool0);
	InputComponent->BindAction("1", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool1);
	InputComponent->BindAction("2", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool2);
	InputComponent->BindAction("3", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool3);
	InputComponent->BindAction("4", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool4);
}

void AUE4VoxelTerrainPlayerController::OnMainActionPressed() {
	SetDestinationPressed();
}

void AUE4VoxelTerrainPlayerController::OnMainActionReleased() {
	SetDestinationReleased();
}

void AUE4VoxelTerrainPlayerController::OnAltActionPressed() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());

	FHitResult Hit = TracePlayerActionPoint();

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

			if (tool_mode == 3) {
				static const float GridRange = 200;
				FVector Tmp(Hit.ImpactPoint);
				Tmp /= GridRange;
				Tmp.Set(std::round(Tmp.X), std::round(Tmp.Y), std::round(Tmp.Z));
				Tmp *= GridRange;
				FVector Position((int)Tmp.X, (int)Tmp.Y, (int)Tmp.Z);

				terrain->digTerrainCubeHole(Position, 110, 5);
			}

			if (tool_mode == 4) {
				terrain->fillTerrainRound(Hit.ImpactPoint, 60, 5, 3); //sand
			}
		}
	}
}

void AUE4VoxelTerrainPlayerController::OnAltActionReleased() {
	GetWorld()->GetTimerManager().ClearTimer(timer);
}

void AUE4VoxelTerrainPlayerController::setTool0() {
	tool_mode = 0;
}

void AUE4VoxelTerrainPlayerController::setTool1() {
	tool_mode = 1;
}

void AUE4VoxelTerrainPlayerController::setTool2() {
	tool_mode = 2;
}

void AUE4VoxelTerrainPlayerController::setTool3() {
	tool_mode = 3;
}

void AUE4VoxelTerrainPlayerController::setTool4() {
	tool_mode = 4;
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


FHitResult AUE4VoxelTerrainPlayerController::TracePlayerActionPoint() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());

	if (pawn->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
		float MaxUseDistance = 800;
		if (pawn->GetCameraBoom() != NULL) {
			MaxUseDistance = pawn->GetCameraBoom()->TargetArmLength + 800;
		}

		FVector CamLoc;
		FRotator CamRot;
		GetPlayerViewPoint(CamLoc, CamRot);

		const FVector StartTrace = CamLoc;
		const FVector Direction = CamRot.Vector();
		const FVector EndTrace = StartTrace + (Direction * MaxUseDistance);

		FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.bTraceComplex = true;
		TraceParams.AddIgnoredActor(pawn);

		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams);

		return Hit;
	}

	if (pawn->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		return Hit;
	}

	return FHitResult();
}