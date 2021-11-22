// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "TerrainController.h"
#include "UnrealSandboxTerrain.h"
#include "CudaTerrainGeneratorComponent.h"

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



#define TotalCacheSize 299593

static const float CaveLayerZ = 7000.f;


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

void UCudaTerrainGeneratorComponent::BeginPlay() {
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("UCudaTerrainGeneratorComponent::BeginPlay"));
}

//==========================================
// Terrain density and material 
//==========================================

bool IsCaveLayerZone(float Z) {
	static constexpr int const ZoneCaveLevel = -(CaveLayerZ / 1000);
	return Z <= ZoneCaveLevel + 1 && Z >= ZoneCaveLevel - 1;
}

void UCudaTerrainGeneratorComponent::PrepareMetaData() {
	UE_LOG(LogTemp, Warning, TEXT("UCudaTerrainGeneratorComponent::PrepareMetaData()"));

	const int Len = 15;

	const FVector O(0, 0, -3000);
	const float ExtendX = ((Len * 1000) / 2) - 100; //1400
	const float ExtendY = 400;
	const float ExtendZ = 400;

	FVector Min(O.X - ExtendX, O.Y - ExtendY, O.Z - ExtendZ);
	FVector Max(O.X + ExtendX, O.Y + ExtendY, O.Z + ExtendZ);
	FBox Box(Min, Max);

	TZoneStructureHandler Str;
	Str.Function = [=](float InDensity, const TVoxelIndex& VoxelIndex, const FVector& LocalPos, const FVector& WorldPos) {
		const float Density = FunctionMakeBox(InDensity, WorldPos, Box);
		const TMaterialId MaterialId = 4;
		return std::make_tuple(Density, MaterialId);
	};

	int X = -(Len / 2) ;
	for (auto I = 0; I < Len; I++) {
		AddZoneStructure(TVoxelIndex(X, 0, -3), Str);
		X++;
	}


	const float ExtendX2 = 400; 
	const float ExtendY2 = ((Len * 1000) / 2) - 100;
	const float ExtendZ2 = 400;

	FVector Min2(O.X - ExtendX2, O.Y - ExtendY2, O.Z - ExtendZ2);
	FVector Max2(O.X + ExtendX2, O.Y + ExtendY2, O.Z + ExtendZ2);
	FBox Box2(Min2, Max2);
	TZoneStructureHandler Str2;
	Str2.Function = [=](float InDensity, const TVoxelIndex& VoxelIndex, const FVector& LocalPos, const FVector& WorldPos) {
		const float Density = FunctionMakeBox(InDensity, WorldPos, Box2);
		const TMaterialId MaterialId = 4;
		return std::make_tuple(Density, MaterialId);
	};

	int Y = -(Len / 2);
	for (auto I = 0; I < Len; I++) {
		AddZoneStructure(TVoxelIndex(0, Y, -3), Str2);
		Y++;
	}
}

bool UCudaTerrainGeneratorComponent::IsForcedComplexZone(const TVoxelIndex& ZoneIndex) {
	if (IsCaveLayerZone(ZoneIndex.Z)) {
		return true;
	}
	
	return Super::IsForcedComplexZone(ZoneIndex);
}

float UCudaTerrainGeneratorComponent::FunctionMakeBox(const float InDensity, const FVector& P, const FBox& InBox) const {
	const float ExtendXP = InBox.Max.X;
	const float ExtendYP = InBox.Max.Y;
	const float ExtendZP = InBox.Max.Z;
	const float ExtendXN = InBox.Min.X;
	const float ExtendYN = InBox.Min.Y;
	const float ExtendZN = InBox.Min.Z;

	static const float E = 50;
	FBox Box = InBox.ExpandBy(E);

	static float D = 100;
	static const float NoisePositionScale = 0.005f;
	static const float NoiseValueScale = 0.1;
	float R = InDensity;

	if (InDensity < 0.5) {
		return InDensity;
	}

	if (FMath::PointBoxIntersection(P, Box)) {
		R = 0;

		if (FMath::Abs(P.X - ExtendXP) < 50 || FMath::Abs(-P.X + ExtendXN) < 50) {
				const float DensityXP = 1 / (1 + exp((ExtendXP - P.X) / D));
				const float DensityXN = 1 / (1 + exp((-ExtendXN + P.X) / D));
				const float DensityX = (DensityXP + DensityXN);
				const float N = PerlinNoise(P, NoisePositionScale, NoiseValueScale);
				R = DensityX + N;
		}

		if (FMath::Abs(P.Y - ExtendYP) < 50 || FMath::Abs(-P.Y + ExtendYN) < 50) {
			if (R < 0.5f) {
				const float DensityYP = 1 / (1 + exp((ExtendYP - P.Y) / D));
				const float DensityYN = 1 / (1 + exp((-ExtendYN + P.Y) / D));
				const float DensityY = (DensityYP + DensityYN);
				const float N = PerlinNoise(P, NoisePositionScale, NoiseValueScale);
				R = DensityY + N;
			}
		}

		if (FMath::Abs(P.Z - ExtendZP) < 50 || FMath::Abs(-P.Z + ExtendZN) < 50) {
			if (R < 0.5f) {
				const float DensityZP = 1 / (1 + exp((ExtendZP - P.Z) / D));
				const float DensityZN = 1 / (1 + exp((-ExtendZN + P.Z) / D));
				const float DensityZ = (DensityZP + DensityZN);
				const float N = PerlinNoise(P, NoisePositionScale, NoiseValueScale / 2);
				R = DensityZ + N;

			}
		}
	}

	if (R > 1) {
		R = 1;
	}

	if (R < 0) {
		R = 0;
	}

	return R;
};


float UCudaTerrainGeneratorComponent::FunctionMakeCaveLayer(float Density, const FVector& WorldPos) const {
	const float BaseCaveLevel = CaveLayerZ;

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
	const float ExtraCaveLevel = 1000 * Noise4;
	const float CaveLevel = BaseCaveLevel + ExtraCaveLevel;

	// cave layer function
	const float CaveHeightF = CaveHeight * 160; // 80000 -> 473
	float CaveLayer = 1 - exp(-pow((WorldPos.Z + CaveLevel), 2) / CaveHeightF); // 80000 -> 473 = 473 * 169.13

	if (WorldPos.Z < -(BaseCaveLevel - 1200) && WorldPos.Z > -(BaseCaveLevel + 1200)) {
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
}

float UCudaTerrainGeneratorComponent::DensityFunctionExt(float Density, const TVoxelIndex& ZoneIndex, const FVector& WorldPos, const FVector& LocalPos) const {

	if (ZoneIndex.X == 0 && ZoneIndex.Y == 0 && ZoneIndex.Z == -1) {
		float Result = Density;

		FVector P = LocalPos;
		float r = std::sqrt(P.X * P.X + P.Y * P.Y + P.Z * P.Z);
		const float MaxR = 1000;
		float t = 1 - exp(-pow(r, 2) / (MaxR * 100));

		//float D = 1 / (1 + exp((r - MaxR) / 10));
		//Result *= t;

		return Result;
	}

	/*
	if (ZoneIndex.X == 0 && ZoneIndex.Y == 0 && ZoneIndex.Z == -3) {
		const float ExtendX = 400;
		const float ExtendY = 400;
		const float ExtendZ = 400;

		FVector Min(-ExtendX, -ExtendY, -ExtendZ);
		FVector Max(ExtendX, ExtendY, ExtendZ);
		FBox Box(Min, Max);
		return FunctionMakeBox(LocalPos, Box);
	}
	*/

	
	if (IsCaveLayerZone(ZoneIndex.Z)) {
		return FunctionMakeCaveLayer(Density, WorldPos);
	}

	return Density;
}

//==========================================
// Foliage (mushrooms)
//==========================================

bool UCudaTerrainGeneratorComponent::UseCustomFoliage(const TVoxelIndex& ZoneIndex) {
	if (ZoneIndex.Z <= -2 && ZoneIndex.Z >= -4) {
		return true;
	}

	return false;
}

FSandboxFoliage UCudaTerrainGeneratorComponent::FoliageExt(const int32 FoliageTypeId, const FSandboxFoliage& FoliageType, const TVoxelIndex& ZoneIndex, const FVector& WorldPos) {
	FSandboxFoliage Foliage = FoliageType;

	// tree
	if (FoliageTypeId >= 100 && FoliageTypeId <= 199) {
		const float R = std::sqrt(WorldPos.X * WorldPos.X + WorldPos.Y * WorldPos.Y + WorldPos.Z * WorldPos.Z);

		if (R < 3500) {
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
			if (T < 0.2) {
				T = 0.2;
			}

			//Foliage.ScaleMaxZ *= T;
			//Foliage.ScaleMinZ = 0.3;
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


void UCudaTerrainGeneratorComponent::PostGenerateNewInstanceObjects(const TVoxelIndex& ZoneIndex, const TVoxelData* Vd, TInstanceMeshTypeMap& ZoneInstanceMeshMap) const {
	FVector ZonePos = Vd->getOrigin();
	if (ZonePos.Z > -3000) {
		return;
	}

	ATerrainController* TerrainController = (ATerrainController*)GetController();
	ALevelController* LevelController = TerrainController->LevelController;

	if (!LevelController) {
		return;
	}

	int32 Hash = ZoneHash(ZonePos);
	FRandomStream Rnd = FRandomStream();
	Rnd.Initialize(Hash);
	Rnd.Reset();

	FVector Pos(0);
	FVector Normal(0);
	TSubclassOf<ASandboxObject> Obj = LevelController->GetSandboxObjectByClassId(300);
	if (SelectRandomSpawnPoint(Rnd, ZoneIndex, Vd, Pos, Normal)) {
		FVector Scale = FVector(1, 1, 1);
		FRotator Rotation = Normal.Rotation();
		Rotation.Pitch -= 90;
		FTransform NewTransform(Rotation, Pos, Scale);
		UWorld* World = TerrainController->GetWorld();
		AsyncTask(ENamedThreads::GameThread, [=]() {
			World->SpawnActor(Obj->ClassDefaultObject->GetClass(), &NewTransform);
		});
	}
}


bool UCudaTerrainGeneratorComponent::SpawnCustomFoliage(const TVoxelIndex& Index, const FVector& WorldPos, int32 FoliageTypeId, FSandboxFoliage FoliageType, FRandomStream& Rnd, FTransform& Transform) {

	/*
	ATerrainController* TerrainController = (ATerrainController*)GetController();
	ALevelController* LevelController = TerrainController->LevelController;

	if (!LevelController) {
		return false;
	}

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
			TSubclassOf<ASandboxObject> Obj = LevelController->GetSandboxObjectByClassId(200);
			float Pitch = Rnd.FRandRange(0.f, 10.f);
			float Roll = Rnd.FRandRange(0.f, 360.f);
			float Yaw = Rnd.FRandRange(0.f, 10.f);
			float ScaleZ = Rnd.FRandRange(FoliageType.ScaleMaxZ, FoliageType.ScaleMaxZ * 2);
			FVector Scale = FVector(ScaleZ, ScaleZ, ScaleZ);
			FRotator Rotation(Pitch, Roll, Yaw);
			FTransform NewTransform(Rotation, Pos, Scale);

			UWorld* World = TerrainController->GetWorld();

			AsyncTask(ENamedThreads::GameThread, [=]() {
				World->SpawnActor(Obj->ClassDefaultObject->GetClass(), &NewTransform);
			});

			return false;
		}

		if (Chance <= Probability) {
			float Angle = Rnd.FRandRange(0.f, 360.f);
			float ScaleZ = Rnd.FRandRange(FoliageType.ScaleMinZ, FoliageType.ScaleMaxZ);
			FVector Scale = FVector(ScaleZ, ScaleZ, ScaleZ);
			Transform = FTransform(FRotator(0, Angle, 0), Pos, Scale);
			return true;
		}
	}
	*/

	return false;
}

void UCudaTerrainGeneratorComponent::BatchGenerateComplexVd(TArray<TGenerateVdTempItm>& GenPass2List) {
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
		UTerrainGeneratorComponent::BatchGenerateComplexVd(GenPass2List);
	}
}

void UCudaTerrainGeneratorComponent::OnBatchGenerationFinished() {

}