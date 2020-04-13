// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SandboxCharacter.h"
#include "SandboxObjectMap.h"
#include "BaseCharacter.generated.h"

UCLASS()
class UE4VOXELTERRAIN_API ABaseCharacter : public ASandboxCharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	USandboxObjectMap* SandboxObjectMap;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UBlueprint* TestObject;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	USoundCue* TestSound;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UStaticMeshComponent* CursorMesh;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
