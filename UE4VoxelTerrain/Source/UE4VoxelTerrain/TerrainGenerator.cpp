// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "TerrainGenerator.h"




void ATerrainGenerator::StartBuildingTerrain() {
	GenerateTerrainMutex.Lock();

	if (!bIsGenerationStarted) {
		bool bShouldGenerateTerrain = true;
		bool bIsMapJsonLoaded = LoadJson();
		if (bIsMapJsonLoaded) {
			FString MapStatus = MapInfo.Status;
			if (MapStatus == "generated") {
				bShouldGenerateTerrain = false;
			}
		}

		if (bShouldGenerateTerrain) {
			// start map generation pipeline
			bIsGenerationStarted = true;

			RunGenerateTerrainPipeline([=] {
				FinishGenerationPipeline();
			}, [=](uint32 Progress, uint32 Total) {
				ProgressGenerationPipeline(Progress, Total);
			});
		} else {
			FinishGenerationPipeline(); 
		}
	}

	GenerateTerrainMutex.Unlock();
}


void ATerrainGenerator::ProgressGenerationPipeline(uint32 Progress, uint32 Total) {
	float PercentProgress = (float)Progress / (float)Total;
	//UE_LOG(LogTemp, Warning, TEXT("ATerrainGenerator::ProgressGenerationPipeline() %d %d %f"), Progress, Total, PercentProgress);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnProgressBuildingTerrain(PercentProgress);
	});
}

void ATerrainGenerator::FinishGenerationPipeline() {
	MapInfo.Status = "generated";

	AsyncTask(ENamedThreads::GameThread, [&]() {
		UE_LOG(LogTemp, Warning, TEXT("ATerrainGenerator::OnFinishBuildingTerrain()"));
		OnFinishBuildingTerrain();
	});
}

void ATerrainGenerator::BeginTerrainLoad() {
	bIsGenerationStarted = false;
}