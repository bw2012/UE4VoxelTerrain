// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "SandboxTerrainController.h"
#include "SandboxCharacter.h"
#include "VoxelIndex.h"
#include "VoxelDualContouringMeshComponent.h"

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
	InputComponent->BindAction("5", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool5);
	InputComponent->BindAction("6", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool6);
	InputComponent->BindAction("7", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool7);
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

		
		UVoxelDualContouringMeshComponent* DualContouringVoxelMesh = Cast<UVoxelDualContouringMeshComponent>(Hit.Component.Get());
		if (DualContouringVoxelMesh != nullptr) {
			if (tool_mode == 1) {
				DualContouringVoxelMesh->EditMeshDeleteSphere(Hit.ImpactPoint, 80, 5);
			}
		}
		
		ASandboxTerrainController* terrain = Cast<ASandboxTerrainController>(Hit.Actor.Get());
		if (terrain != NULL) {
			TVoxelIndex ZoneIndex = terrain->GetZoneIndex(Hit.ImpactPoint);
			FVector ZoneIndexTmp(ZoneIndex.X, ZoneIndex.Y, ZoneIndex.Z);

			UE_LOG(LogTemp, Warning, TEXT("zIndex -> %f %f %f"), ZoneIndexTmp.X, ZoneIndexTmp.Y, ZoneIndexTmp.Z);

			if (tool_mode == 1) {
				terrain->DigTerrainRoundHole(Hit.ImpactPoint, 80, 5);
				GetWorld()->GetTimerManager().SetTimer(timer, this, &AUE4VoxelTerrainPlayerController::PerformAction, 0.1, true);
			}

			if (tool_mode == 2) {
				terrain->DigTerrainCubeHole(Hit.ImpactPoint, 110);
			}

			if (tool_mode == 3) {
				static const float GridRange = 100;
				FVector Tmp(Hit.ImpactPoint);
				Tmp /= GridRange;
				Tmp.Set(std::round(Tmp.X), std::round(Tmp.Y), std::round(Tmp.Z));
				Tmp *= GridRange;
				FVector Position((int)Tmp.X, (int)Tmp.Y, (int)Tmp.Z);

				terrain->DigTerrainCubeHole(Position, 110);
			}

			if (tool_mode == 4) {
				terrain->FillTerrainRound(Hit.ImpactPoint, 60, 3); //sand
			}

			if (tool_mode == 5) {
				terrain->FillTerrainRound(Hit.ImpactPoint, 60, 4); //basalt
			}

			if (tool_mode == 6) {
				terrain->FillTerrainRound(Hit.ImpactPoint, 60, 5); //gravel
			}

			if (tool_mode == 7) {
				static const float GridRange = 100;
				FVector Tmp(Hit.ImpactPoint);
				Tmp /= GridRange;
				Tmp.Set(std::round(Tmp.X), std::round(Tmp.Y), std::floor(Tmp.Z));
				Tmp *= GridRange;
				FVector Position((int)Tmp.X, (int)Tmp.Y, ((int)Tmp.Z) + GridRange);

				terrain->FillTerrainCube(Position, 55, 4); // basalt
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

void AUE4VoxelTerrainPlayerController::setTool5() {
	tool_mode = 5;
}

void AUE4VoxelTerrainPlayerController::setTool6() {
	tool_mode = 6;
}

void AUE4VoxelTerrainPlayerController::setTool7() {
	tool_mode = 7;
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
				terrain->DigTerrainRoundHole(Hit.ImpactPoint, 80, 5);
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