// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SandboxObject.h"
#include "Ramp.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API ARamp : public ASandboxObject {
	GENERATED_BODY()
	
public:
	virtual uint64 GetSandboxTypeId();
};
