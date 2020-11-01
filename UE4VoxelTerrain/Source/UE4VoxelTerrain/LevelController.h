// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SandboxLevelController.h"
#include "LevelController.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API ALevelController : public ASandboxLevelController
{
	GENERATED_BODY()


public:

	TSubclassOf<ASandboxObject> GetSandboxObjectByClassId(int32 ClassId);
	
};
