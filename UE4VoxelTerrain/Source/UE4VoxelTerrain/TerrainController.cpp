// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"

#include "UnrealSandboxTerrain.h"
#include "TerrainController.h"

#include "SandboxObject.h"
#include "DrawDebugHelpers.h"

#include "CudaTerrainGeneratorComponent.h"


//======================================================================================================================================================================
// Terrain generator 
//======================================================================================================================================================================


class UCuda2TerrainGeneratorComponent : public UTerrainGeneratorComponent {

public:

	//UCudaTerrainGeneratorComponent::UCudaTerrainGeneratorComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
		//UE_LOG(LogTemp, Warning, TEXT(" UCudaTerrainGeneratorComponent"));
	//}

	
};


UTerrainGeneratorComponent* ATerrainController::NewTerrainGenerator() {
	UCudaTerrainGeneratorComponent* CudaGenerator = NewObject<UCudaTerrainGeneratorComponent>(this, TEXT("TerrainGenerator"));
	UE_LOG(LogTemp, Warning, TEXT("TEST -> %s"), *CudaGenerator->GetClass()->GetName());
	return CudaGenerator;
}

void ATerrainController::OnOverlapActorDuringTerrainEdit(const FHitResult& OverlapResult, const FVector& Pos) {
	ASandboxObject* Object = Cast<ASandboxObject>(OverlapResult.GetActor());
	if (Object) {
		UStaticMeshComponent* Component = Cast<UStaticMeshComponent>(Object->GetRootComponent());
		if (Component) {
			//Component->SetSimulatePhysics(true);
		}
	}
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