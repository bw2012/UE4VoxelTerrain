// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "PreparationHelperActor.h"

// Sets default values
APreparationHelperActor::APreparationHelperActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool CheckSaveDirLocal(FString SaveDir) {
	UE_LOG(LogTemp, Log, TEXT("Check directory: %s"), *SaveDir);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*SaveDir)) {
		PlatformFile.CreateDirectory(*SaveDir);
		UE_LOG(LogTemp, Log, TEXT("Create directory: %s"), *SaveDir);
		if (!PlatformFile.DirectoryExists(*SaveDir)) {
			UE_LOG(LogTemp, Warning, TEXT("Unable to create directory: %s"), *SaveDir);
			return false;
		}
	}

	return true;
}

// Called when the game starts or when spawned
void APreparationHelperActor::BeginPlay()
{
	Super::BeginPlay();

	// TODO finish
	FString MapName = TEXT("World 0");
	FString SavePath = FPaths::ProjectSavedDir();
	FString SaveDir = SavePath + TEXT("Map/");

	if (!CheckSaveDirLocal(SaveDir)) {
		// log error
	}

	FString SaveDirWorld0 = SaveDir + MapName + TEXT("/");
	if (!CheckSaveDirLocal(SaveDirWorld0)) {
		// log error
	}
}

// Called every frame
void APreparationHelperActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

