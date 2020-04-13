
#pragma once
#include "UE4VoxelTerrain.h"
#include "SandboxObject.h"
#include "SandboxCharacter.h"
#include "SandboxTerrainController.h"
#include "BaseCharacter.h"


FVector SnapToGrid(const FVector& Location, const float GridRange) {
	FVector Tmp(Location);
	Tmp /= GridRange;
	Tmp.Set(std::round(Tmp.X), std::round(Tmp.Y), std::round(Tmp.Z));
	Tmp *= GridRange;
	return FVector((int)Tmp.X, (int)Tmp.Y, (int)Tmp.Z);
}

bool IsCursorPositionValid(const FHitResult& Hit) {
	if (Hit.bBlockingHit) {
		ACharacter* Character = Cast<ACharacter>(Hit.Actor);
		if (Character) {
			return false;
		}

		return true;
	}

	return false;
}

void TransformPos(FVector& Pos, FRotator& Rotation, const FHitResult& TraceResult) {
	ASandboxObject* SandboxObject = Cast<ASandboxObject>(TraceResult.Actor);
	if (SandboxObject && SandboxObject->GetSandboxTypeId() == 10) {
		FVector TargetActorPos = TraceResult.Actor->GetActorLocation();
		Pos = TargetActorPos;
		Rotation = TraceResult.Actor->GetActorRotation();

		FVector Normal = TraceResult.ImpactNormal;
		Pos = Pos + (Normal * 60);
	}
}

void CalculateCursorPosition(ABaseCharacter* Character, const FHitResult& Res, FVector& Location, FRotator& Rotation, ASandboxObject* Obj) {
	if (Character->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		Location = Res.Location;
		Rotation = FRotator();
	}

	if (Character->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
		FRotator Rot = Character->FollowCamera->GetComponentRotation();
		Rot.Pitch = 0;
		Rot.Roll = 0;
		Location = Res.Location;
		Rotation = Rot;
	}

	if (Character->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) {
		FRotator Rot = Character->GetCapsuleComponent()->GetComponentRotation();
		Location = Res.Location;
		Rotation = Rot;
	}

	if (Obj->GetSandboxTypeId() == 10) { // cube + cube
		TransformPos(Location, Rotation, Res);
	}
}

void DigTerrainAround(ASandboxObject* Object, UWorld* World, FVector Pos) {
	TArray<UStaticMeshComponent*> Components;
	Object->GetComponents<UStaticMeshComponent>(Components);
	for (UStaticMeshComponent* StaticMeshComponent : Components) {
		FBoxSphereBounds BoxSphereBounds = StaticMeshComponent->CalcBounds(FTransform());
		TArray<FOverlapResult> OverlapArray;
		World->OverlapMultiByChannel(OverlapArray, Pos, FQuat(), ECC_Visibility, FCollisionShape::MakeSphere(BoxSphereBounds.SphereRadius));
		for (auto& Overlap : OverlapArray) {
			ASandboxTerrainController* Terrain = Cast<ASandboxTerrainController>(Overlap.Actor.Get());
			if (Terrain) {
				Terrain->DigTerrainCubeHole(Pos, 30);
				continue;
			}
		}
	}
}

FRotator SelectRotation() {
	const auto DirIndex = FMath::RandRange(0, 6);
	static const FRotator Direction[7] = { FRotator(0), FRotator(90, 0, 0), FRotator(-90, 0, 0), FRotator(0, 90, 0), FRotator(0, -90, 0), FRotator(0, 0, 90), FRotator(0, 0, -90) };
	const FRotator Rotation = Direction[DirIndex];

	return Rotation;
}


void SpawnSandboxObject(UWorld* World, ASandboxCharacter* Owner, const FVector& Location, const FRotator& Rotation, ASandboxObject* Object) {
	//UE_LOG(LogTemp, Warning, TEXT("test actor -> %s"), *Test);
	//UE_LOG(LogTemp, Warning, TEXT("test normal -> %f %f %f"), TraceResult.ImpactNormal.X, TraceResult.ImpactNormal.Y, TraceResult.ImpactNormal.Z);

	auto SpawnActorClass = Object->GetClass();
	if (SpawnActorClass) {
		if (Object->GetSandboxTypeId() == 10) {
			//DigTerrainAround(Object, World, Location);
		}

		FRotator RotationExt;
		if (Object->GetSandboxTypeId() == 10) {
			RotationExt = SelectRotation();
		}

		RotationExt += Rotation;
		AActor* NewActor = World->SpawnActor(SpawnActorClass, &Location, &RotationExt);
	}

	return;
};