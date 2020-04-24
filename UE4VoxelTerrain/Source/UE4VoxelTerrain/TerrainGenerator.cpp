// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "TerrainGenerator.h"




void ATerrainGenerator::StartTerrainGeneration() {
	GenerateTerrainMutex.Lock();
	if (!bIsGenerationStarted) {
		UE_LOG(LogTemp, Warning, TEXT("ATerrainGenerator::StartTerrainGeneration()"));
		bIsGenerationStarted = true;

		/*
		if (ASandboxTerrainController::OpenFile()) {
			ASandboxTerrainController::RunLoadMapAsync([&](){
				UE_LOG(LogTemp, Warning, TEXT("TEST"));
				//GetWorld()->ServerTravel(FString("/Game/Test/TestMap"));
				FFunctionGraphTask::CreateAndDispatchWhenReady([=]() { GetWorld()->ServerTravel(FString("/Game/Test/TestMap"));  }, TStatId(), nullptr, ENamedThreads::GameThread);
			});
		}
		*/
	}
	GenerateTerrainMutex.Unlock();
}

void ATerrainGenerator::BeginPlayServer() {
	bIsGenerationStarted = false;
}