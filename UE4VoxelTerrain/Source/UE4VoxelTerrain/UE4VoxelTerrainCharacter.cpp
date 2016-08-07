// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainCharacter.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "DrawDebugHelpers.h"

AUE4VoxelTerrainCharacter::AUE4VoxelTerrainCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void drawSelection(UWorld* w, FVector v) {
	static const float s = 105;
	static const float t = 3;
	static const FColor clr(255, 255, 255, 100);

	DrawDebugLine(w, FVector(v.X - s, v.Y - s, v.Z + s), FVector(v.X + s, v.Y - s, v.Z + s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X + s, v.Y - s, v.Z + s), FVector(v.X + s, v.Y + s, v.Z + s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X + s, v.Y + s, v.Z + s), FVector(v.X - s, v.Y + s, v.Z + s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X - s, v.Y + s, v.Z + s), FVector(v.X - s, v.Y - s, v.Z + s), clr, false, -1, 0, t);

	DrawDebugLine(w, FVector(v.X - s, v.Y - s, v.Z - s), FVector(v.X + s, v.Y - s, v.Z - s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X + s, v.Y - s, v.Z - s), FVector(v.X + s, v.Y + s, v.Z - s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X + s, v.Y + s, v.Z - s), FVector(v.X - s, v.Y + s, v.Z - s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X - s, v.Y + s, v.Z - s), FVector(v.X - s, v.Y - s, v.Z - s), clr, false, -1, 0, t);

	DrawDebugLine(w, FVector(v.X - s, v.Y - s, v.Z - s), FVector(v.X - s, v.Y - s, v.Z + s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X + s, v.Y - s, v.Z - s), FVector(v.X + s, v.Y - s, v.Z + s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X + s, v.Y + s, v.Z - s), FVector(v.X + s, v.Y + s, v.Z + s), clr, false, -1, 0, t);
	DrawDebugLine(w, FVector(v.X - s, v.Y + s, v.Z - s), FVector(v.X - s, v.Y + s, v.Z + s), clr, false, -1, 0, t);
}

void AUE4VoxelTerrainCharacter::Tick(float DeltaSeconds)
{
	if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);

			AUE4VoxelTerrainPlayerController* controller = Cast<AUE4VoxelTerrainPlayerController>(GetController());

			if (controller -> tool_mode == 1) {
				DrawDebugSphere(GetWorld(), TraceHitResult.Location, 80, 24, FColor(255, 255, 255, 100));
			}

			if (controller->tool_mode == 2) {
				drawSelection(GetWorld(), TraceHitResult.Location);
			}


		}
	}
}
