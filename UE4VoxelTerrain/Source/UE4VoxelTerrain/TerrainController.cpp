// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"

#include "UnrealSandboxTerrain.h"
#include "TerrainController.h"
#include "SandboxTerrainGenerator.h"

#include "SandboxObject.h"
#include "DrawDebugHelpers.h"



//#define USBT_ZONE_SIZE			1000.f
//#define USBT_ZONE_DIMENSION		65

typedef struct TZoneData {
	float x;
	float y;
	float z;

	int c[10];

} TZoneData;

typedef struct TVdGenBlock {
	int size = 0;

	TZoneData* zoneData = nullptr;
	size_t zd_size = 0;

	TDensityVal* voxelData = nullptr;
	size_t vd_size = 0;

	TMaterialId* materialData = nullptr;
	size_t md_size = 0;

	int* cacheData = nullptr;
	size_t cd_size = 0;

} TVdGenBlock;


typedef int(*PCudaGetInfo)();
typedef int(*PCudaGenerateVd)(TVdGenBlock*);

#define TotalCacheSize 299593


class TVdGenBlockCuda {

public:

	TVdGenBlockCuda(int size) {
		vdGenBlk.size = size;
		int vd_full_num = USBT_ZONE_DIMENSION * USBT_ZONE_DIMENSION * USBT_ZONE_DIMENSION;

		vdGenBlk.vd_size = vd_full_num * sizeof(TDensityVal) * size;
		vdGenBlk.md_size = vd_full_num * sizeof(TMaterialId) * size;
		vdGenBlk.zd_size = size * sizeof(TZoneData);
		vdGenBlk.cd_size = TotalCacheSize * sizeof(int) * size;

		vdGenBlk.voxelData = (TDensityVal*)malloc(vdGenBlk.vd_size);
		vdGenBlk.materialData = (TMaterialId*)malloc(vdGenBlk.md_size);
		vdGenBlk.zoneData = (TZoneData*)malloc(vdGenBlk.zd_size);
		vdGenBlk.cacheData = (int*)malloc(vdGenBlk.cd_size);
	}

	~TVdGenBlockCuda() {
		free(vdGenBlk.voxelData);
		free(vdGenBlk.materialData);
		free(vdGenBlk.zoneData);
		free(vdGenBlk.cacheData);
	}

	TVdGenBlock* GetVdGenBlock() {
		return &vdGenBlk;
	}

private:

	TVdGenBlock vdGenBlk;

};


//======================================================================================================================================================================
// Terrain generator 
//======================================================================================================================================================================


class TCudaTerrainGenerator : public TDefaultTerrainGenerator {
	//using TDefaultTerrainGenerator::TDefaultTerrainGenerator;

public:


	TCudaTerrainGenerator(ASandboxTerrainController* Controller) : TDefaultTerrainGenerator(Controller) {

	};

	//==========================================
	// Terrain density and material 
	//==========================================


	bool ForcePerformZone(const TVoxelIndex& ZoneIndex) {
		// cave level z=3000 ~ ZoneIndex.Z = 3 +- 1
		if (ZoneIndex.Z <= -2 && ZoneIndex.Z >= -4) {
			return true;
		}

		if (ZoneIndex.X == 0 && ZoneIndex.Y == 0 && ZoneIndex.Z == -1) {
			return true;
		}

		return false;
	}

	float DensityFunctionExt(float Density, const TVoxelIndex& ZoneIndex, const FVector& WorldPos, const FVector& LocalPos) const {

		if (ZoneIndex.X == 0 && ZoneIndex.Y == 0 && ZoneIndex.Z == -1) {
			float Result = Density;

			FVector P = LocalPos;
			float r = std::sqrt(P.X * P.X + P.Y * P.Y + P.Z * P.Z);
			const float MaxR = 1000;
			float t = 1 - exp(-pow(r, 2) / ( MaxR * 100));

			//float D = 1 / (1 + exp((r - MaxR) / 10));

			Result *= t;

			return Result;
		}

		if (ZoneIndex.Z <= -2 && ZoneIndex.Z >= -4) {
			float Result = Density;

			static const float scale0 = 0.005f; // small
			static const float scale1 = 0.001f; // small
			static const float scale2 = 0.0003f; // big

			const FVector v0(WorldPos.X * scale0, WorldPos.Y * scale0, WorldPos.Z * 0.0002); // Stalactite
			const FVector v1(WorldPos.X * scale1, WorldPos.Y * scale1, WorldPos.Z * scale1); // just noise
			//const FVector v2(WorldPos.X * scale2, WorldPos.Y *scale2, WorldPos.Z * scale2); // just noise big
			const float Noise0 = PerlinNoise(v0);
			const float Noise1 = PerlinNoise(v1);
			//const float Noise2 = PerlinNoise(v2);
			//const float Noise2 = PerlinNoise(v2);

			//const FVector v4(WorldPos.X * scale3, WorldPos.Y * scale3, 10); // extra cave level
			//const float Noise4 = PerlinNoise(v4);

			//float NormalizedPerlin = (NoiseMedium + 0.87) / 1.73;
			//float Z = WorldPos.Z + NoiseMedium * 100;
			//float DensityByGroundLevel = 1 - (1 / (1 + exp(-Z)));

			// cave height
			static const float scale3 = 0.001f;
			const FVector v3(WorldPos.X * scale3, WorldPos.Y * scale3, 0); // extra cave height
			const float Noise3 = PerlinNoise(v3);
			const float BaseCaveHeight = 400;
			const float ExtraCaveHeight = 490 * Noise3;
			float CaveHeight = BaseCaveHeight + ExtraCaveHeight;
			//float CaveHeight = BaseCaveHeight;
			if (CaveHeight < 0) {
				CaveHeight = 0; // protection if my calculation is failed
			}

			//cave level
			static const float scale4 = 0.00025f;
			const FVector v4(WorldPos.X * scale4, WorldPos.Y * scale4, 10); // extra cave height
			const float Noise4 = PerlinNoise(v4);
			const float BaseCaveLevel = 3000;
			const float ExtraCaveLevel = 1000 * Noise4;
			const float CaveLevel = BaseCaveLevel + ExtraCaveLevel;

			// cave layer function
			const float CaveHeightF = CaveHeight * 160; // 80000 -> 473
			float CaveLayer = 1 - exp(-pow((WorldPos.Z + CaveLevel), 2) / CaveHeightF); // 80000 -> 473 = 473 * 169.13

			if (WorldPos.Z < -1800 && WorldPos.Z > -4200) {
				const float CaveLayerN = 1 - CaveLayer;
				//Result *= CaveLayer + CaveLayerN * Noise0 * 0.5 + Noise1 * 0.75 + Noise2 * 0.5;
				Result *= CaveLayer + CaveLayerN * Noise0 * 0.5 + Noise1 * 0.75;
				//Result *= CaveLayer;
			}

			//Result = Density * (NoiseMedium * 0.5 + t);

			if (Result < 0) {
				Result = 0;
			}

			if (Result > 1) {
				Result = 1;
			}

			return Result;
		} else {
			return Density;
		}
	}

	//==========================================
	// Foliage (mushrooms)
	//==========================================

	bool  UseCustomFoliage(const TVoxelIndex& ZoneIndex) override {
		if (ZoneIndex.Z <= -2 && ZoneIndex.Z >= -4) {
			return true;
		}

		return false;
	}

	FSandboxFoliage FoliageExt(const int32 FoliageTypeId, const FSandboxFoliage& FoliageType, const TVoxelIndex& ZoneIndex, const FVector& WorldPos) {
		FSandboxFoliage Foliage = FoliageType;

		// tree
		if (FoliageTypeId >= 100 && FoliageTypeId <= 199) {
			const float R = std::sqrt(WorldPos.X * WorldPos.X + WorldPos.Y * WorldPos.Y + WorldPos.Z * WorldPos.Z);

			if (R < 3500 ) {
				Foliage.Probability *= 0.01;
			}

			if (R > 8000) {
				//forest
				Foliage.Probability *= 20;
				Foliage.SpawnStep *= 0.7;
				Foliage.OffsetRange *= 0.7;
			}
		}

		// grass
		if (FoliageTypeId == 1) {
			const float R = std::sqrt(WorldPos.X * WorldPos.X + WorldPos.Y * WorldPos.Y + WorldPos.Z * WorldPos.Z);

			if (R < 1000) {
				float T = R / 1000;
				if(T < 0.2) {
					T = 0.2;
				}

				Foliage.ScaleMaxZ *= T;
				Foliage.ScaleMinZ = 0.3;
			}
		}

		if (FoliageTypeId == 2) {
			const float R = std::sqrt(WorldPos.X * WorldPos.X + WorldPos.Y * WorldPos.Y + WorldPos.Z * WorldPos.Z);

			if (R < 500) {
				Foliage.Probability = 0;
			}
		}

		return Foliage;
	}


	bool SpawnCustomFoliage(const TVoxelIndex& Index, const FVector& WorldPos, int32 FoliageTypeId, FSandboxFoliage FoliageType, FRandomStream& Rnd, FTransform& Transform) override {

		ATerrainController* TerrainController = (ATerrainController*)GetController();
		ALevelController* LevelController = TerrainController->LevelController;

		FVector Pos = WorldPos;

		// cave height
		static const float scale3 = 0.001f;
		const FVector v3(WorldPos.X * scale3, WorldPos.Y * scale3, 0); // extra cave height
		const float Noise3 = PerlinNoise(v3);
		const float BaseCaveHeight = 400;
		const float ExtraCaveHeight = 490 * Noise3;
		float CaveHeight = BaseCaveHeight + ExtraCaveHeight;
		//float CaveHeight = BaseCaveHeight;
		if (CaveHeight < 0) {
			CaveHeight = 0; // protection if my calculation is failed
		}

		//cave level
		static const float scale4 = 0.00025f;
		const FVector v4(WorldPos.X * scale4, WorldPos.Y * scale4, 10); // extra cave height
		const float Noise4 = PerlinNoise(v4);
		const float BaseCaveLevel = 3000;
		const float ExtraCaveLevel = 1000 * Noise4;
		const float CaveLevel = BaseCaveLevel + ExtraCaveLevel;

		static const float scale1 = 0.001f; // small
		const FVector v1(WorldPos.X * scale1, WorldPos.Y * scale1, 0); // just noise
		const float Noise1 = PerlinNoise(v1);

		float test = (-CaveLevel - CaveHeight / 2);
		if (WorldPos.Z < test - 200 && WorldPos.Z > test - 220) { // && WorldPos.Z > test - 105
			//FVector rrr = WorldPos;
			//rrr.Z += 10;
			//if (Noise1 > 0) {
			//	Foliage.Probability = 0;
			//}


			float Chance = Rnd.FRandRange(0.f, 1.f);
			Pos.Z += 20;

			float Probability = FoliageType.Probability;
			if (Chance <= Probability / 20) {
				if (LevelController) {
					TSubclassOf<ASandboxObject> Obj = LevelController->GetSandboxObjectByClassId(200);
					float Pitch = Rnd.FRandRange(0.f, 10.f);
					float Roll = Rnd.FRandRange(0.f, 360.f);
					float Yaw = Rnd.FRandRange(0.f, 10.f);
					float ScaleZ = Rnd.FRandRange(FoliageType.ScaleMaxZ, FoliageType.ScaleMaxZ * 2);
					FVector Scale = FVector(ScaleZ, ScaleZ, ScaleZ);
					FRotator Rotation(Pitch, Roll, Yaw);
					FTransform Transform(Rotation, Pos, Scale);

					UWorld* World = TerrainController->GetWorld();

					AsyncTask(ENamedThreads::GameThread, [=]() {
						World->SpawnActor(Obj->ClassDefaultObject->GetClass(), &Transform);
						});
					return false;
				}
			}

			if (Chance <= Probability) {
				float Angle = Rnd.FRandRange(0.f, 360.f);
				float ScaleZ = Rnd.FRandRange(FoliageType.ScaleMinZ, FoliageType.ScaleMaxZ);
				FVector Scale = FVector(ScaleZ, ScaleZ, ScaleZ);
				Transform = FTransform(FRotator(0, Angle, 0), Pos, Scale);
				return true;
			}
		}

		return false;
	}


protected:

	void BatchGenerateComplexVd(TArray<TGenerateVdTempItm>& GenPass2List) override {
		ATerrainController* Controller = (ATerrainController*)GetController();
		if (Controller->IsUseCuda()) {

			int TotalZoneCount = GenPass2List.Num();
			if (TotalZoneCount == 0) {
				return;
			}

			UE_LOG(LogTemp, Log, TEXT("Use CUDA: %d"), GenPass2List.Num());

			double Start = FPlatformTime::Seconds();

			TVdGenBlockCuda VdGenBlockCuda(TotalZoneCount);
			TVdGenBlock* vdGenBlk = VdGenBlockCuda.GetVdGenBlock();

			int i = 0;
			const static int s = USBT_ZONE_DIMENSION * USBT_ZONE_DIMENSION * USBT_ZONE_DIMENSION;
			for (const auto& P : GenPass2List) {
				FVector Pos = Controller->GetZonePos(P.ZoneIndex);
				vdGenBlk->zoneData[i].x = Pos.X;
				vdGenBlk->zoneData[i].y = Pos.Y;
				vdGenBlk->zoneData[i].z = Pos.Z;
				i++;
			}

			PCudaGenerateVd CudaGenerateVd = (PCudaGenerateVd)Controller->CudaGenVdPtr;
			int CudaResult = CudaGenerateVd(vdGenBlk);

			int idx = 0;
			for (auto& P : GenPass2List) {
				P.Vd->copyDataUnsafe(&vdGenBlk->voxelData[idx * s], &vdGenBlk->materialData[idx * s]);
				P.Vd->clearSubstanceCache();
				P.Vd->copyCacheUnsafe(&vdGenBlk->cacheData[idx * s], vdGenBlk->zoneData[idx].c);
				idx++;
			}
			
			double End = FPlatformTime::Seconds();
			double Time = (End - Start) * 1000;
			UE_LOG(LogTemp, Log, TEXT("CudaResult -> %d ----> %f ms"), CudaResult, Time);

		} else {
			TDefaultTerrainGenerator::BatchGenerateComplexVd(GenPass2List);
		}
	}

	void OnBatchGenerationFinished() override {

	}
};

TBaseTerrainGenerator* ATerrainController::NewTerrainGenerator() {
 	return new TCudaTerrainGenerator(this);
}

void ATerrainController::OnOverlapActorDuringTerrainEdit(const FHitResult& OverlapResult, const FVector& Pos) {
	ASandboxObject* Object = Cast<ASandboxObject>(OverlapResult.GetActor());
	if (Object) {
		UStaticMeshComponent* RootComponent = Cast<UStaticMeshComponent>(Object->GetRootComponent());
		if (RootComponent) {
			//RootComponent->SetSimulatePhysics(true);
		}
	}
}


void ATerrainController::BatchGenerateNewVd(const TArray<TSpawnZoneParam>& GenerationList, TArray<TVoxelData*>& NewVdArray) {
	Super::BatchGenerateNewVd(GenerationList, NewVdArray);
}

bool ATerrainController::IsUseCuda() {
	if (CudaGenVdPtr && bUseCUDAGenerator) {
		return true;
	}

	return false;
}


void ATerrainController::BeginPlay() {
	if (bUseCUDAGenerator) {
		//FString filePath = *FPaths::GamePluginsDir() + folder + "/" + name;
		FString CudaGenDllFilePath = "D:\\src\\bin\\win64\\Debug\\CudaVdGenerator.dll";
		if (FPaths::FileExists(CudaGenDllFilePath)) {
			this->CudaGenDllHandle = FPlatformProcess::GetDllHandle(*CudaGenDllFilePath);
			if (this->CudaGenDllHandle) {
				UE_LOG(LogTemp, Log, TEXT("CudaVdGenerator.dll -> LOADED"));

				FString CudaGetInfoName = "CudaGetInfo";
				PCudaGetInfo CudaGetInfo = (PCudaGetInfo)FPlatformProcess::GetDllExport(this->CudaGenDllHandle, *CudaGetInfoName);
				if (CudaGetInfo) {
					UE_LOG(LogTemp, Log, TEXT("CudaGetInfo -> OK"));
				}

				FString CudaGenerateVdName = "CudaGenerateVd";
				PCudaGenerateVd CudaGenerateVd = (PCudaGenerateVd)FPlatformProcess::GetDllExport(this->CudaGenDllHandle, *CudaGenerateVdName);
				if (CudaGenerateVd) {
					UE_LOG(LogTemp, Log, TEXT("CudaGenerateVd -> OK"));

					CudaGenVdPtr = CudaGenerateVd;
				}
			}
			else {
				UE_LOG(LogTemp, Log, TEXT("Error loading CudaVdGenerator.dll"));
			}
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("CudaVdGenerator.dll not found"));
		}
	}

	
	Super::BeginPlay();
}

void ATerrainController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (this->CudaGenDllHandle) {
		UE_LOG(LogTemp, Log, TEXT("Free CudaVdGenerator.dll"));
		FPlatformProcess::FreeDllHandle(CudaGenDllHandle);
	}

}