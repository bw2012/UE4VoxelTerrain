// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "SandboxCharacter.h"


// Sets default values
ASandboxCharacter::ASandboxCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASandboxCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASandboxCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ASandboxCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("ZoomIn", IE_Released, this, &ASandboxCharacter::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Released, this, &ASandboxCharacter::ZoomOut);

}

void ASandboxCharacter::ZoomIn() {
	if (GetCameraBoom() == NULL) return;
	if (GetCameraBoom()->TargetArmLength > 200) {
		GetCameraBoom()->TargetArmLength -= 100;
	}

	UE_LOG(LogTemp, Warning, TEXT("ZoomIn: %f"), GetCameraBoom()->TargetArmLength);
}

void ASandboxCharacter::ZoomOut() {
	if (GetCameraBoom() == NULL) return;
	if (GetCameraBoom()->TargetArmLength < 1500) {
		GetCameraBoom()->TargetArmLength += 100;
	}

	UE_LOG(LogTemp, Warning, TEXT("ZoomOut: %f"), GetCameraBoom()->TargetArmLength);
}