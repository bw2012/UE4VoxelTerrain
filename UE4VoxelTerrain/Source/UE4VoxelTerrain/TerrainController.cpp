// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "TerrainController.h"
#include "SandboxObject.h"
#include "DrawDebugHelpers.h"
#include "VoxelData.h"


FORCEINLINE float ATerrainController::GeneratorDensityFunc(const TVoxelDensityFunctionData& FunctionData) {
	const TVoxelIndex& ZoneIndex = FunctionData.ZoneIndex;
	if (ZoneIndex.Z <= -2 && ZoneIndex.Z >= -4) {
		const FVector& WorldPos = FunctionData.WorldPos;
		float Density = FunctionData.Density;
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
		return FunctionData.Density;
	}
}

bool ATerrainController::GeneratorForcePerformZone(const TVoxelIndex& ZoneIndex) {

	// cave level z=3000 ~ ZoneIndex.Z = 3 +- 1
	if (ZoneIndex.Z <= -2 && ZoneIndex.Z >= -4) {
		return true;
	}

	return false;
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

FSandboxFoliage ATerrainController::GeneratorFoliageOverride(const int32 FoliageTypeId, const FSandboxFoliage& FoliageType, const TVoxelIndex& ZoneIndex, const FVector& WorldPos) {
	FSandboxFoliage Foliage = FoliageType;

	// tree
	if (FoliageTypeId == 100) {
		const float R = std::sqrt(WorldPos.X * WorldPos.X + WorldPos.Y * WorldPos.Y + WorldPos.Z * WorldPos.Z);

		if (R > 8000) {
			Foliage.Probability *= 20;
			Foliage.SpawnStep *= 0.7;
			Foliage.OffsetRange *= 0.7;
		}
	}

	return Foliage;
}

FORCEINLINE bool  ATerrainController::GeneratorUseCustomFoliage(const TVoxelIndex& ZoneIndex) {
	if (ZoneIndex.Z <= -2 && ZoneIndex.Z >= -4) {
		return true;
	}

	return false;
}


FORCEINLINE bool ATerrainController::GeneratorSpawnCustomFoliage(const TVoxelIndex& Index, const FVector& WorldPos, int32 FoliageTypeId, FSandboxFoliage FoliageType, FRandomStream& Rnd, FTransform& Transform) {
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
				AsyncTask(ENamedThreads::GameThread, [=]() {
					GetWorld()->SpawnActor(Obj->ClassDefaultObject->GetClass(), &Transform);
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