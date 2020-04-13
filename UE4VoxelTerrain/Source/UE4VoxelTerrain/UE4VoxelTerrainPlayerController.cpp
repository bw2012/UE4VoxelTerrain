// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "SandboxTerrainController.h"
#include "SandboxCharacter.h"
#include "VoxelIndex.h"
#include "VoxelDualContouringMeshComponent.h"

#include "SpawnHelper.hpp"

// test only
#include "SandboxObject.h"
#include "BaseCharacter.h"
#include "DrawDebugHelpers.h"


#define DIG_CUBE_SIZE 110 //60
#define DIG_CUBE_SNAP_TO_GRID 100 //50

AUE4VoxelTerrainPlayerController::AUE4VoxelTerrainPlayerController() {
	DiggingToolMode = 0;
	bIsConstructionMode = false;
}

void AUE4VoxelTerrainPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);
}

void AUE4VoxelTerrainPlayerController::SetupInputComponent() {
	// set up gameplay key bindings
	Super::SetupInputComponent();
}

void AUE4VoxelTerrainPlayerController::OnMainActionPressed() {
	SetDestinationPressed();
}

void AUE4VoxelTerrainPlayerController::OnMainActionReleased() {
	SetDestinationReleased();
}

void DigTerrain() {

}

ASandboxObject* AUE4VoxelTerrainPlayerController::GetCurrentInventoryObject() {
	UContainerComponent* Inventory = GetInventory();
	if (Inventory != nullptr) {
		FContainerStack* Stack = Inventory->GetSlot(CurrentInventorySlot);
		if (Stack != nullptr) {
			if (Stack->Amount > 0) {
				TSubclassOf<ASandboxObject>	ObjectClass = Stack->ObjectClass;
				if (ObjectClass != nullptr) {
					ASandboxObject* Actor = Cast<ASandboxObject>(ObjectClass->ClassDefaultObject);
					return Actor;
				}
			}
		}
	}

	return nullptr;
}

void AUE4VoxelTerrainPlayerController::OnAltActionPressed() {
	ABaseCharacter* Character = Cast<ABaseCharacter>(GetCharacter());
	if (Character->IsDead()) return;

	FHitResult Hit = TracePlayerActionPoint();

	if (Hit.bBlockingHit) {
		UE_LOG(LogTemp, Warning, TEXT("test point -> %f %f %f"), Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z);

		if (bIsConstructionMode) {
			ASandboxObject* Obj = GetCurrentInventoryObject();
			if (IsCursorPositionValid(Hit) && Obj) {
				FVector Location;
				FRotator Rotation;

				CalculateCursorPosition(Character, Hit, Location, Rotation, Obj);
				SpawnSandboxObject(GetWorld(), Character, Location, Rotation, Obj);
			}
		} else {
			ASandboxTerrainController* Terrain = Cast<ASandboxTerrainController>(Hit.Actor.Get());
			if (Terrain) {
				TVoxelIndex ZoneIndex = Terrain->GetZoneIndex(Hit.ImpactPoint);
				FVector ZoneIndexTmp(ZoneIndex.X, ZoneIndex.Y, ZoneIndex.Z);
				UE_LOG(LogTemp, Warning, TEXT("zIndex -> %f %f %f"), ZoneIndexTmp.X, ZoneIndexTmp.Y, ZoneIndexTmp.Z);

				if (DiggingToolMode == 0) {
					Terrain->DigTerrainRoundHole(Hit.ImpactPoint, 80, 5);
					GetWorld()->GetTimerManager().SetTimer(Timer, this, &AUE4VoxelTerrainPlayerController::PerformAction, 0.1, true);
				}

				if (DiggingToolMode == 1) {
					FVector Location = SnapToGrid(Hit.Location, DIG_CUBE_SNAP_TO_GRID);
					Terrain->DigTerrainCubeHole(Location, DIG_CUBE_SIZE);

					float test = 55;

					FVector Min(-test);
					FVector Max(test);
					FBox Box(Min, Max);
					//Terrain->DigTerrainCubeHole(Location, Box, -45, FVector(0, 0, 1));
					//Terrain->DigTerrainCubeHole(Location, Box, 0, FVector(0, 0, 1));

					ABaseCharacter* TestCharacter = Cast<ABaseCharacter>(GetCharacter());
					if (TestCharacter) {
						if (TestCharacter->TestSound) {
							UGameplayStatics::PlaySoundAtLocation(GetWorld(), TestCharacter->TestSound, Hit.ImpactPoint, FRotator(0));
						}
					}
				}
			}
		}


		/*
		ASandboxTerrainController* terrain = Cast<ASandboxTerrainController>(Hit.Actor.Get());
		if (terrain != NULL) {
			TVoxelIndex ZoneIndex = terrain->GetZoneIndex(Hit.ImpactPoint);
			FVector ZoneIndexTmp(ZoneIndex.X, ZoneIndex.Y, ZoneIndex.Z);

			UE_LOG(LogTemp, Warning, TEXT("zIndex -> %f %f %f"), ZoneIndexTmp.X, ZoneIndexTmp.Y, ZoneIndexTmp.Z);

			tool_mode = 3;

			if (tool_mode == 1) {
				terrain->DigTerrainRoundHole(Hit.ImpactPoint, 80, 5);
				GetWorld()->GetTimerManager().SetTimer(timer, this, &AUE4VoxelTerrainPlayerController::PerformAction, 0.1, true);
			}

			if (tool_mode == 2) {
                terrain->DigTerrainCubeHole(Hit.ImpactPoint, 110);
				ABaseCharacter* TestCharacter = Cast<ABaseCharacter>(GetCharacter());
				if (TestCharacter) {
					if (TestCharacter->TestSound) {
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), TestCharacter->TestSound, Hit.ImpactPoint, FRotator(0));
					}
				}
			}

			if (tool_mode == 3) {
				static const float GridRange = 100;
				FVector Tmp(Hit.ImpactPoint);
				Tmp /= GridRange;
				Tmp.Set(std::round(Tmp.X), std::round(Tmp.Y), std::round(Tmp.Z));
				Tmp *= GridRange;
				FVector Position((int)Tmp.X, (int)Tmp.Y, (int)Tmp.Z);

				terrain->DigTerrainCubeHole(Position, 60);
                
                
                FVector Min(-100, 0, 0);
                FVector Max(100, 200, 100);
                FBox Box(Min, Max);
                //FTransform Transform(FQuat(FVector(1, 0, 0), -45), TraceHitResult.Location, FVector(1));
                
                //terrain->DigTerrainCubeHole(Hit.ImpactPoint, 110);
                //terrain->DigTerrainCubeHole(Position, Box, -45, FVector(1, 0, 0));
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
		}*/
	}
}

void AUE4VoxelTerrainPlayerController::OnAltActionReleased() {
	GetWorld()->GetTimerManager().ClearTimer(Timer);
}

void AUE4VoxelTerrainPlayerController::PerformAction() {
	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());
	if (Character->GetSandboxPlayerView() != PlayerView::TOP_DOWN) { return; }

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);
	if (Hit.bBlockingHit) {
		ASandboxTerrainController* Terrain = Cast<ASandboxTerrainController>(Hit.Actor.Get());
		if (Terrain) {
			if (DiggingToolMode == 0) {
				Terrain->DigTerrainRoundHole(Hit.ImpactPoint, 80, 5);
			}
		}
	}
}

void AUE4VoxelTerrainPlayerController::OnTracePlayerActionPoint(const FHitResult& Res) {
	ABaseCharacter* Character = Cast<ABaseCharacter>(GetCharacter());
	if (Character->IsDead()) return;
	if (Character) {
		ASandboxObject* Obj = GetCurrentInventoryObject();
		if (bIsConstructionMode && IsCursorPositionValid(Res) && Obj) {
			auto Mesh = Obj->SandboxRootMesh->GetStaticMesh();
			Character->CursorMesh->SetStaticMesh(Mesh);
			Character->CursorMesh->SetVisibility(true, true);

			FVector Location;
			FRotator Rotation;
			CalculateCursorPosition(Character, Res, Location, Rotation, Obj);
			Character->CursorMesh->SetWorldLocationAndRotationNoPhysics(Location, Rotation);
		} else {
			Character->CursorMesh->SetVisibility(false, true);
			Character->CursorMesh->SetStaticMesh(nullptr);

			if (!bIsConstructionMode) {
				ASandboxTerrainController* TerrainController = Cast<ASandboxTerrainController>(Res.Actor.Get());
				if (TerrainController) {
					if (DiggingToolMode == 0) {
						DrawDebugSphere(GetWorld(), Res.Location, 80, 24, FColor(255, 255, 255, 100));
					}

					if (DiggingToolMode == 1) {
						FVector Location = SnapToGrid(Res.Location, DIG_CUBE_SNAP_TO_GRID);
						DrawDebugBox(GetWorld(), Location, FVector(DIG_CUBE_SIZE), FColor(255, 255, 255, 100));
					}
				}
			}
		}
	}
}