// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "TerrainGenerator.h"

#define MessageTime 10.0f


class TBaseGenerator;

void ATerrainGenerator::StartBuildingTerrain() {
	GenerateTerrainMutex.Lock();

	if (!bIsGenerationStarted) {
		bool bShouldGenerateTerrain = true;

		const int TerrainSizeMinZ = -5;
		const int TerrainSizeMaxZ = 5;
		const uint32 AreaRadius = AreaGenerationSize;
		const uint32 TotalChunks = (AreaRadius * 2 + 1) * (AreaRadius * 2 + 1);
		const uint32 Total = (AreaRadius * 2 + 1) * (AreaRadius * 2 + 1) * (-TerrainSizeMinZ + TerrainSizeMaxZ + 1);
		auto List = MakeChunkListByAreaSize(AreaRadius);

		UE_LOG(LogTemp, Log, TEXT("Total chunks: %d"), TotalChunks);
		GEngine->AddOnScreenDebugMessage(-1, MessageTime, FColor::Silver, FString::Printf(TEXT("Total chunks: %d"), TotalChunks));

		TArray<TVoxelIndex> NewVdGenList;
		for (auto& ChunkIndex : List) {
			for (auto Z = TerrainSizeMaxZ; Z >= TerrainSizeMinZ; Z--) {
				TVoxelIndex ZoneIndex(ChunkIndex.X, ChunkIndex.Y, Z);
				bool bIsVdExists = IsVdExistsInFile(ZoneIndex);
				if (!bIsVdExists) {
					NewVdGenList.Add(ZoneIndex);
				}
			}
		}

		const int MaxBatchCount = FGenericPlatformMath::CeilToInt((float)NewVdGenList.Num() / (float)this->BatchSize);
		UE_LOG(LogTemp, Log, TEXT("NewVdGenerationList: %d"), NewVdGenList.Num());
		UE_LOG(LogTemp, Log, TEXT("Batches: %d, BatchSize: %d"), MaxBatchCount, this->BatchSize);

		RunThread([=]() {
			const unsigned int T = NewVdGenList.Num();
			const unsigned int BatchSize = this->BatchSize;

			int Idx = 0;
			int IdxB = 1;
			int FinishedZonesCount = 0;

			TArray<TSpawnZoneParam> NewVdBatchList;

			for (auto& ZoneIndex : NewVdGenList) {
				TSpawnZoneParam P(ZoneIndex);
				NewVdBatchList.Add(P);

				if (IsWorkFinished()) {
					break;
				}

				if ((Idx > 0 && Idx % BatchSize == 0) || (Idx == T-1)) {
					double Start = FPlatformTime::Seconds();

					//GEngine->AddOnScreenDebugMessage(-1, MessageTime, FColor::Silver, FString::Printf(TEXT("%d / %d"), IdxB, MaxBatchCount));
					//UE_LOG(LogTemp, Log, TEXT("batch %d / %d"), IdxB, MaxBatchCount);


					TArray<TGenerateZoneResult> NewVdArray;
					//BatchGenerateNewVd(NewVdBatchList, NewVdArray);
					GetTerrainGenerator()->BatchGenerateVoxelTerrain(NewVdBatchList, NewVdArray);

					double End = FPlatformTime::Seconds();
					double Time = (End - Start) * 1000;
					//UE_LOG(LogTemp, Log, TEXT("batch generation ----> %f ms"), Time);

					//GEngine->AddOnScreenDebugMessage(-1, MessageTime, FColor::Silver, FString::Printf(TEXT("Save terrain...")));

					int Idx2 = 0;
					double Start2 = FPlatformTime::Seconds();
					/*
					for (TVoxelData* Vd : NewVdArray) {
						const TVoxelIndex& ZoneIndex = NewVdBatchList[Idx2].Index;
						TInstanceMeshTypeMap ObjectMap;

						TMeshDataPtr Md = nullptr;
						//if (Vd->getDensityFillState() == TVoxelDataFillState::MIXED) {
							//TInstanceMeshTypeMap ObjectMap;						
							//Md = GenerateMesh(Vd);
							//GetTerrainGenerator()->GenerateInstanceObjects(ZoneIndex, Vd, ObjectMap);
						//}

						ForceSave(ZoneIndex, Vd, Md, ObjectMap);
						delete Vd;

						ProgressGenerationPipeline(FinishedZonesCount, T);

						Idx2++;
						FinishedZonesCount++;
					}
					*/

					double End2 = FPlatformTime::Seconds();
					double Time2 = (End2 - Start2) * 1000;
					//UE_LOG(LogTemp, Log, TEXT(" ----> %f ms"), Time2);

					//GetTerrainGenerator()->Clean(); // TODO generator
					NewVdBatchList.Empty();
					IdxB++;
				}

				Idx++;
			}

			UE_LOG(LogTemp, Log, TEXT("FINISH"));
			FinishGenerationPipeline();
		});
	}

	GenerateTerrainMutex.Unlock();
}


void ATerrainGenerator::ProgressGenerationPipeline(uint32 Progress, uint32 Total) {
	float PercentProgress = (float)Progress / (float)Total;
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnProgressBuildingTerrain(PercentProgress);
	});
}

void ATerrainGenerator::FinishGenerationPipeline() {
	//MapInfo.Status = "generated";

	AsyncTask(ENamedThreads::GameThread, [&]() {
		OnProgressBuildingTerrain(1.f);
		OnFinishBuildingTerrain();
	});
}

void ATerrainGenerator::BeginTerrainLoad() {
	bIsGenerationStarted = false;
}