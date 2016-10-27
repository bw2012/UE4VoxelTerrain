// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainGameMode.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "UE4VoxelTerrainCharacter.h"
#include "MainHUD.h"

AUE4VoxelTerrainGameMode::AUE4VoxelTerrainGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AUE4VoxelTerrainPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = AMainHUD::StaticClass();
}