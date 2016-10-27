// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "MainHUD.h"
#include "UI/SystemInfoWidget.h"


void AMainHUD::BeginPlay() {
	openWidget<UUserWidget>(TEXT("/Game/Sandbox/UI/W_SystemInfo.W_SystemInfo_C"));
}


template<typename T>
T* AMainHUD::openWidget(FString widget_name) {
	FStringClassReference big_inventory_widget_ref(widget_name);
	if (UClass* w = big_inventory_widget_ref.TryLoadClass<T>()) {
		T* widget = CreateWidget<T>(GetWorld(), w);
		widget->AddToViewport();
		return widget;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("widget %s is not found"), *widget_name);
		return NULL;
	}
}

