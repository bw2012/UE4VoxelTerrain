// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "SandboxTerrainController.h"
#include "SandboxCharacter.h"
#include "VoxelIndex.h"

#include "SpawnHelper.hpp"

// test only
#include "SandboxObject.h"
#include "BaseCharacter.h"
#include "MiningTool.h"
#include "DrawDebugHelpers.h"


AUE4VoxelTerrainPlayerController::AUE4VoxelTerrainPlayerController() {
	bPlaceCurrentObjectToWorld = false;
}

void AUE4VoxelTerrainPlayerController::BeginPlay() {
	Super::BeginPlay();

	for (TActorIterator<ASandboxEnvironment> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ASandboxEnvironment* Env = Cast<ASandboxEnvironment>(*ActorItr);
		if (Env) {
			UE_LOG(LogTemp, Log, TEXT("Found ASandboxEnvironment -> %s"), *Env->GetName());
			SandboxEnvironment = Env;
			break;
		}
	}

	for (TActorIterator<ALevelController> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ALevelController* LevelCtrl = Cast<ALevelController>(*ActorItr);
		if (LevelCtrl) {
			UE_LOG(LogTemp, Log, TEXT("Found ALevelController -> %s"), *LevelCtrl->GetName());
			LevelController = LevelCtrl;
			break;
		}
	}

	for (TActorIterator<ATerrainController> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ATerrainController* TerrainCtrl = Cast<ATerrainController>(*ActorItr);
		if (TerrainCtrl) {
			UE_LOG(LogTemp, Log, TEXT("Found ATerrainController -> %s"), *TerrainCtrl->GetName());
			TerrainController = TerrainCtrl;
			break;
		}
	}
}

void AUE4VoxelTerrainPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	ABaseCharacter* Character = Cast<ABaseCharacter>(GetCharacter());
	if (Character) {
		FVector Location = Character->GetActorLocation();
		const float Distance = FVector::Distance(Location, PrevLocation);
		if (Distance > 50) {
			PrevLocation = Location;
			// update player position
			SandboxEnvironment->UpdatePlayerPosition(Location);
			if (SandboxEnvironment) {
				if (Location.Z < -500) {
					//UE_LOG(LogSandboxTerrain, Log, TEXT("SetCaveMode = true"));
					SandboxEnvironment->SetCaveMode(true);
				} else {
					//UE_LOG(LogSandboxTerrain, Log, TEXT("SetCaveMode = false"));
					SandboxEnvironment->SetCaveMode(false);
				}
			}
		}
	}
}

void AUE4VoxelTerrainPlayerController::ToggleToolMode() { 
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetCharacter());
	if (!BaseCharacter) {
		return;
	}

	if (BaseCharacter->IsDead()) {
		return;
	}

	ASandboxObject* Obj = GetCurrentInventoryObject();
	if (Obj) {
		if (Obj->GetSandboxTypeId() == 100) { // if shovel
			AMiningTool* MiningTool = Cast<AMiningTool>(Obj);
			if (MiningTool) {
				MiningTool->ToggleToolMode();
			}
		}
	}
};

void AUE4VoxelTerrainPlayerController::SetupInputComponent() {
	// set up gameplay key bindings
	Super::SetupInputComponent();
}

void AUE4VoxelTerrainPlayerController::OnMainActionPressed() {
	SetDestinationPressed();
	bPlaceCurrentObjectToWorld = false;
}

void AUE4VoxelTerrainPlayerController::OnMainActionReleased() {
	SetDestinationReleased();
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
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetCharacter());
	if (!BaseCharacter) {
		return;
	}

	if (BaseCharacter->IsDead()) {
		return;
	}

	FHitResult Hit = TracePlayerActionPoint();
	if (Hit.bBlockingHit) {
		//UE_LOG(LogSandboxTerrain, Warning, TEXT("test point -> %f %f %f"), Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z);

		ASandboxObject* Obj = GetCurrentInventoryObject();
		if (Obj) {
			if (Obj->GetSandboxTypeId() == 100) { // if shovel
				AMiningTool* MiningTool = Cast<AMiningTool>(Obj);
				if (MiningTool) {
					MiningTool->OnAltAction(Hit, BaseCharacter);
				}
				return;
			}

			if (bPlaceCurrentObjectToWorld) {
				if (IsCursorPositionValid(Hit) && Obj) {
					FVector Location;
					FRotator Rotation;

					CalculateCursorPosition(BaseCharacter, Hit, Location, Rotation, Obj);
					SpawnSandboxObject(GetWorld(), BaseCharacter, Location, Rotation, Obj);
					//DropSandboxObject(GetWorld(), Character, Obj);
					return;
				}
			} else {
				bPlaceCurrentObjectToWorld = true;
			}
		}
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
			//if (DiggingToolMode == 0) {
				//Terrain->DigTerrainRoundHole(Hit.ImpactPoint, 80, 5);
			//}
		}
	}
}

void AUE4VoxelTerrainPlayerController::OnTracePlayerActionPoint(const FHitResult& Res) {
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetCharacter());
	if (BaseCharacter) {
		if (BaseCharacter->IsDead()) {
			return;
		}

		ASandboxObject* Obj = GetCurrentInventoryObject();
		if (Obj) {
			if (Obj->GetSandboxTypeId() == 100) { //shovel
				AMiningTool* MiningTool = Cast<AMiningTool>(Obj);
				if (MiningTool) {
					MiningTool->OnTracePlayerActionPoint(Res, BaseCharacter);
				}
			}
		}

		if (bPlaceCurrentObjectToWorld && IsCursorPositionValid(Res) && Obj) {
			auto Mesh = Obj->SandboxRootMesh->GetStaticMesh();
			FVector Scale = Obj->GetRootComponent()->GetRelativeScale3D();
			//UE_LOG(LogSandboxTerrain, Warning, TEXT("Scale = %f %f %f"), Scale.X, Scale.Y, Scale.Z);
			BaseCharacter->CursorMesh->SetStaticMesh(Mesh);
			BaseCharacter->CursorMesh->SetVisibility(true, true);
			BaseCharacter->CursorMesh->SetRelativeScale3D(Scale);

			FVector Location;
			FRotator Rotation;
			CalculateCursorPosition(BaseCharacter, Res, Location, Rotation, Obj);
			BaseCharacter->CursorMesh->SetWorldLocationAndRotationNoPhysics(Location, Rotation);
		} else {
			BaseCharacter->CursorMesh->SetVisibility(false, true);
			BaseCharacter->CursorMesh->SetStaticMesh(nullptr);
			BaseCharacter->CursorMesh->SetRelativeScale3D(FVector(1));
		}
	}
}