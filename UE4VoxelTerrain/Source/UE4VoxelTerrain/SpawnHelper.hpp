
#pragma once
#include "UE4VoxelTerrain.h"
#include "SandboxObject.h"
#include "SandboxCharacter.h"
#include "SandboxTerrainController.h"
#include "BaseCharacter.h"


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

void TransformPos(ASandboxObject* NewObj, FVector& Pos, FRotator& Rotation, const FHitResult& TraceResult) {
	ASandboxObject* TargetObject = Cast<ASandboxObject>(TraceResult.Actor);

	// cube
	if (NewObj->GetSandboxTypeId() == 10) {
		if (TargetObject && TargetObject->GetSandboxTypeId() == 10) {
			FVector TargetActorPos = TraceResult.Actor->GetActorLocation();
			Pos = TargetActorPos;
			Rotation = TraceResult.Actor->GetActorRotation();

			FVector Normal = TraceResult.ImpactNormal;
			Pos = Pos + (Normal * 60);
		} else {
			FVector Normal = TraceResult.ImpactNormal;
			Pos = Pos + (Normal * 30);
		}

		return;
	}
	
}

void CalculateTargetRotation(ABaseCharacter* Character, FRotator& Rotation) {
	if (Character->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		Rotation = FRotator();
	}

	if (Character->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
		FRotator Rot = Character->FollowCamera->GetComponentRotation();
		Rot.Pitch = 0;
		Rot.Roll = 0;
		Rotation = Rot;
	}

	if (Character->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) {
		FRotator Rot = Character->GetCapsuleComponent()->GetComponentRotation();
		Rotation = Rot;
	}
}

void CalculateCursorPosition(ABaseCharacter* Character, const FHitResult& Res, FVector& Location, FRotator& Rotation, ASandboxObject* Obj) {
	Location = Res.Location;
	CalculateTargetRotation(Character, Rotation);
	TransformPos(Obj, Location, Rotation, Res);
}


void DigTerrainAround(ASandboxObject* Object, UWorld* World, const FVector& Pos, const FRotator& Rotator) {
	TArray<UStaticMeshComponent*> Components;
	Object->GetComponents<UStaticMeshComponent>(Components);
	for (UStaticMeshComponent* StaticMeshComponent : Components) {
		FBoxSphereBounds BoxSphereBounds = StaticMeshComponent->CalcBounds(FTransform());
		TArray<FOverlapResult> OverlapArray;
		World->OverlapMultiByChannel(OverlapArray, Pos, FQuat(), ECC_Visibility, FCollisionShape::MakeSphere(BoxSphereBounds.SphereRadius));
		for (auto& Overlap : OverlapArray) {
			ASandboxTerrainController* Terrain = Cast<ASandboxTerrainController>(Overlap.Actor.Get());
			if (Terrain) {
				Terrain->DigTerrainCubeHole(Pos, 30, Rotator);
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
	auto SpawnActorClass = Object->GetClass();
	if (SpawnActorClass) {
		FRotator RotationExt(0);
		if (Object->GetSandboxTypeId() == 10) {
			RotationExt = SelectRotation();
		}

		RotationExt += Rotation;

		if (Object->GetSandboxTypeId() == 10) {
			DigTerrainAround(Object, World, Location, RotationExt);
		}

		AActor* NewActor = World->SpawnActor(SpawnActorClass, &Location, &RotationExt);
	}
};

void DropSandboxObject(UWorld* World, ASandboxCharacter* Owner, ASandboxObject* Object) {
	auto SpawnActorClass = Object->GetClass();
	//const FRotator SpawnRotation = Owner->GetControlRotation();
	const FRotator SpawnRotation = Owner->GetActorRotation();
	const FVector SpawnLocation = Owner->GetActorLocation() + SpawnRotation.RotateVector(FVector(100, 0, 50));

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	AActor* NewActor = World->SpawnActor(SpawnActorClass, &SpawnLocation, &SpawnRotation, ActorSpawnParams);
	if (NewActor) {
		UStaticMeshComponent* RootComponent = Cast<UStaticMeshComponent>(NewActor->GetRootComponent());
		if (RootComponent) {
			RootComponent->SetSimulatePhysics(true);
			//RootComponent->SetMassScale(NAME_None, 2);
			RootComponent->SetPhysicsLinearVelocity(SpawnRotation.RotateVector(FVector(500, 0, 0)));
		}
	}

};



void HelpDigStairs(const FVector& Location, const FRotator& PlayerRotation, TUniqueFunction<void(const FVector& TmpPos)> Function) {
	FVector V = PlayerRotation.RotateVector(FVector(1, 0, 0));
	V.Z = 0;
	V.Normalize(0.001f);
	V.X = FMath::RoundHalfToZero(V.X);
	V.Y = FMath::RoundHalfToZero(V.Y);
	V.Normalize(0.001f);

	if (V.X == 0 && V.Y == 0) {
		return;
	}

	float Test = FMath::Abs(FMath::Abs(V.X) - FMath::Abs(V.Y));

	if (Test < 0.1f) {
		return;
	}

	float Step = 25;
	FVector Tmp(Location);
	Tmp.Z += 150;
	for (int i = 0; i < 5; i++) {
		Tmp += V * 25 * 2;
		Tmp.Z -= Step;
		//DrawDebugBox(GetWorld(), Tmp, FVector(DIG_CUBE_SIZE), FColor(255, 255, 255, 100));
		Function(Tmp);
	}
}
