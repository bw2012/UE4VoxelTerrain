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


	//GetMesh()->AttachParent = RootComponent;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	view = PlayerView::TOP_DOWN;
	initTopDownView();

	// Create a decal in the world to show the cursor's location
	//CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	//CursorToWorld->AttachTo(RootComponent);
	//static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	//if (DecalMaterialAsset.Succeeded())
	//{
	//	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	//}
	//CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	//CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AUE4VoxelTerrainCharacter::Tick(float DeltaSeconds)
{
	if (view != PlayerView::TOP_DOWN) {
		return;
	}

	//if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_WorldStatic, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			//CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			//CursorToWorld->SetWorldRotation(CursorR);

			if (!TraceHitResult.bBlockingHit) {
				return;
			}

			AUE4VoxelTerrainPlayerController* controller = Cast<AUE4VoxelTerrainPlayerController>(GetController());

			if (controller -> tool_mode == 1) {
				DrawDebugSphere(GetWorld(), TraceHitResult.Location, 80, 24, FColor(255, 255, 255, 100));
			}

			if (controller->tool_mode == 2) {
				DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(105), FColor(255, 255, 255, 100));
			}


		}
	}
}
