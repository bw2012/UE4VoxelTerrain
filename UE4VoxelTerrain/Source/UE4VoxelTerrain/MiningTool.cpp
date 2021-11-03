// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "MiningTool.h"

//#include "SpawnHelper.hpp"
#include "VoxelMeshComponent.h"


#define Dig_Cube_Size 110 //60
#define Dig_Snap_To_Grid 100 //50

#define Dig_Cylinder_Length 100
#define Dig_Cylinder_Radius 100

//FIXME
void CalculateTargetRotation(ABaseCharacter* Character, FRotator& Rotation);


FVector SnapToGrid(const FVector& Location, const float GridRange) {
	FVector Tmp(Location);
	Tmp /= GridRange;
	Tmp.Set(std::round(Tmp.X), std::round(Tmp.Y), std::round(Tmp.Z));
	Tmp *= GridRange;
	return FVector((int)Tmp.X, (int)Tmp.Y, (int)Tmp.Z);
}

void HelpDigCylinder(ABaseCharacter* Character, const FVector& Location, const float Len, TUniqueFunction<void(const FVector& Pos, const FVector& LenMin, const FVector& LenMax, const FRotator& Rotation)> Function) {
	const FVector& Tmp = Location;
	FRotator Rotation;
	CalculateTargetRotation(Character, Rotation);
	//Rotation += FRotator(45, 0, 0);
	FVector LenV(0, 0, Len);
	LenV = Rotation.RotateVector(LenV);

	FVector LenMin = Tmp - LenV;
	FVector LenMax = Tmp + LenV;

	Function(Tmp, LenMin, LenMax, Rotation);
}



AMiningTool::AMiningTool() {
	DiggingToolMode = 0;
}


void AMiningTool::PlaySound(const FSandboxTerrainMaterial& MatInfo, const FVector& Location, UWorld* World) {
	if (this->DiggingRockSound && MatInfo.Type == FSandboxTerrainMaterialType::Rock) {
		UGameplayStatics::PlaySoundAtLocation(World, this->DiggingRockSound, Location, FRotator(0));
	}

	if (this->DiggingSoilSound && MatInfo.Type == FSandboxTerrainMaterialType::Soil) {
		UGameplayStatics::PlaySoundAtLocation(World, this->DiggingSoilSound, Location, FRotator(0));
	}
}


void AMiningTool::OnAltAction(const FHitResult& Hit, ABaseCharacter* Owner) {
	UWorld* World = Owner->GetWorld();
	ASandboxTerrainController* Terrain = Cast<ASandboxTerrainController>(Hit.Actor.Get());
	if (Terrain) {
		TVoxelIndex ZoneIndex = Terrain->GetZoneIndex(Hit.ImpactPoint);
		FVector ZoneIndexTmp(ZoneIndex.X, ZoneIndex.Y, ZoneIndex.Z);

		UVoxelMeshComponent* ZoneMesh = Cast<UVoxelMeshComponent>(Hit.Component.Get());
		if (ZoneMesh) {
			uint16 MatId = ZoneMesh->GetMaterialIdFromCollisionFaceIndex(Hit.FaceIndex);
			//UE_LOG(LogSandboxTerrain, Log, TEXT("MatId -> %d"), MatId);

			if (MatId > 0) {
				FSandboxTerrainMaterial MatInfo;
				if (Terrain->GetTerrainMaterialInfoById(MatId, MatInfo)) {
					PlaySound(MatInfo, Hit.Location, World);
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Test -> %f %f %f"), Hit.Location.X, Hit.Location.Y, Hit.Location.Z);
		UE_LOG(LogTemp, Log, TEXT("ZoneIndex -> %f %f %f"), ZoneIndexTmp.X, ZoneIndexTmp.Y, ZoneIndexTmp.Z);

		if (DiggingToolMode == 0) {
			Terrain->DigTerrainRoundHole(Hit.ImpactPoint, 80, 5);
			if (Owner->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
				//GetWorld()->GetTimerManager().SetTimer(Timer, this, &AUE4VoxelTerrainPlayerController::PerformAction, 0.1, true);
			}
		}

		if (DiggingToolMode == 1) {
			FVector Location = SnapToGrid(Hit.Location, Dig_Snap_To_Grid);
			Terrain->DigTerrainCubeHole(Location, Dig_Cube_Size);

			//float test = 55;
			//FVector Min(-test);
			//FVector Max(test);
			//FBox Box(Min, Max);
			//Terrain->DigTerrainCubeHole(Location, Box, -45, FVector(0, 0, 1));
			//Terrain->DigTerrainCubeHole(Location, Box, 0, FVector(0, 0, 1));
		}

		if (DiggingToolMode == 2) {
			//Terrain->FillTerrainRound(Hit.ImpactPoint, 60, 3); //sand
			Terrain->FillTerrainRound(Hit.ImpactPoint, 60, 1); //dirt
		}

		
		if (DiggingToolMode == 3) {
			//const FVector Location = SnapToGrid(Hit.Location, Dig_Snap_To_Grid);
			const FVector Location = Hit.Location;
			const float Len = Dig_Cylinder_Length;
			HelpDigCylinder(Owner, Location, Len, [&](const FVector& Pos, const FVector& LenMin, const FVector& LenMax, const FRotator& Rotation) {
				Terrain->DigCylinder(Hit.ImpactPoint, Dig_Cylinder_Radius, Len, Rotation);
			});

			/*
			const FVector Location = SnapToGrid(Hit.Location, 100);
			FBox MyBox = FBox(FVector(0, -110, -105), FVector(200, 105, 400));
			Terrain->DigTerrainCubeHole(Location, MyBox, Dig_Cube_Size);

			if (LevelController) {
				// spawn ramp
				FVector RampPos = Location;
				RampPos.Z = Hit.Location.Z - 10.f;
				TSubclassOf<ASandboxObject> RampObj = LevelController->GetSandboxObjectByClassId(110);
				FVector Scale = FVector(1, 1, 1);
				FRotator Rotation(-26.5f, 0, 0);
				FTransform Transform(Rotation, RampPos, Scale);
				GetWorld()->SpawnActor(RampObj->ClassDefaultObject->GetClass(), &Transform);
			}
			*/
		}
	}

	/*
	if (DiggingToolMode == 3) {
		ASandboxObject* TargetObj = Cast<ASandboxObject>(Hit.Actor.Get());
		if (TargetObj && TargetObj->GetSandboxTypeId() == 110) {
			FVector RampPos = TargetObj->GetActorLocation();
			FVector D(200, 0, -100);
			RampPos += D;

			FBox MyBox = FBox(FVector(0, -110, -105), FVector(200, 105, 250));
			TerrainController->DigTerrainCubeHole(RampPos, MyBox, Dig_Cube_Size);
			TSubclassOf<ASandboxObject> RampObj = LevelController->GetSandboxObjectByClassId(110);
			FVector Scale = FVector(1, 1, 1);
			FRotator Rotation(-26.5f, 0, 0);
			FTransform Transform(Rotation, RampPos, Scale);
			GetWorld()->SpawnActor(RampObj->ClassDefaultObject->GetClass(), &Transform);
		}
	}
	*/


}

void AMiningTool::OnTracePlayerActionPoint(const FHitResult& Res, ABaseCharacter* Owner) {
	UWorld* World = Owner->GetWorld();
	ASandboxTerrainController* TerrainController = Cast<ASandboxTerrainController>(Res.Actor.Get());
	if (TerrainController) {
		if (DiggingToolMode == 0) {
			DrawDebugSphere(World, Res.Location, 80, 24, FColor(255, 255, 255, 100));
		}

		if (DiggingToolMode == 1) {
			const FVector Location = SnapToGrid(Res.Location, Dig_Snap_To_Grid);
			DrawDebugBox(World, Location, FVector(Dig_Cube_Size), FColor(255, 255, 255, 100));
		}

		if (DiggingToolMode == 2) {
			DrawDebugSphere(World, Res.Location, 60, 24, FColor(100, 255, 255, 100));
		}

		if (DiggingToolMode == 3) {
			//const FVector Location = SnapToGrid(Hit.Location, Dig_Snap_To_Grid);
			const FVector Location = Res.Location;
			const float Len = Dig_Cylinder_Length;
			HelpDigCylinder(Owner, Location, Len, [&](const FVector& Pos, const FVector& LenMin, const FVector& LenMax, const FRotator& Rotation) {
				DrawDebugCylinder(World, LenMax, LenMin, Dig_Cylinder_Radius, 128, FColor(255, 255, 255, 128));
			});
		}
	}


	/*
const FVector Location = SnapToGrid(Res.Location, Dig_Snap_To_Grid);
const FRotator Rotation = GetControlRotation();
HelpDigStairs(Location, Rotation, [&](const FVector& TmpPos) {
	DrawDebugBox(GetWorld(), TmpPos, FVector(Dig_Cube_Size), FColor(255, 255, 255, 100));
});
*/

/*
const FVector Location = SnapToGrid(Res.Location, 100);
FBox MyBox = FBox(FVector(0, -105, -105), FVector(200, 105, 200));
FTransform Transform(Location);
DrawDebugSolidBox(World, MyBox, FColor(255, 255, 255, 100), Transform, false);
*/


	/*
	if (DiggingToolMode == 3) {
		ASandboxObject* TargetObj = Cast<ASandboxObject>(Res.Actor.Get());
		if (TargetObj && TargetObj->GetSandboxTypeId() == 110) {
			FVector Pos = TargetObj->GetActorLocation();
			FVector D(200, 0, -100);
			Pos += D;

			FBox MyBox = FBox(FVector(0, -105, -105), FVector(200, 105, 200));
			FTransform Transform(Pos);
			DrawDebugSolidBox(GetWorld(), MyBox, FColor(255, 255, 255, 100), Transform, false);
		}
	}
	*/
}