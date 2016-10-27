// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void BeginPlay();


private:
	template<typename T>
	T* openWidget(FString widget_name);
	
	
};
