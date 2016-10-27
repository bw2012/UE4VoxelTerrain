// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "Blueprint/UserWidget.h"
#include "SystemInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXELTERRAIN_API USystemInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable, Category = "SandboxHUD")
	FString sandboxSystemInfoText();

	UFUNCTION(BlueprintCallable, Category = "SandboxHUD")
	float sandboxSystemInfoPercent();

	UFUNCTION(BlueprintCallable, Category = "SandboxHUD")
	ESlateVisibility sandboxSystemInfoVisiblity();
	
	
};
