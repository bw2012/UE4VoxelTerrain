// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "SandboxCharacter.h"
#include "UE4VoxelTerrainCharacter.generated.h"

UCLASS(Blueprintable)
class AUE4VoxelTerrainCharacter : public ASandboxCharacter
{
	GENERATED_BODY()

public:
	AUE4VoxelTerrainCharacter();

	virtual void Tick(float DeltaSeconds) override;

private:


};

