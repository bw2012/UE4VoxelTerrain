// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	CursorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CursorMesh"));
	CursorMesh->SetRelativeLocation(FVector(400.0f, 0.f, 0.f));
	CursorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CursorMesh->SetCollisionProfileName(TEXT("NoCollision"));
	CursorMesh->SetCanEverAffectNavigation(false);
	CursorMesh->SetVisibleFlag(false);
	CursorMesh->AttachTo(FollowCamera);
	CursorMesh->SetWorldRotation(FRotator());
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

