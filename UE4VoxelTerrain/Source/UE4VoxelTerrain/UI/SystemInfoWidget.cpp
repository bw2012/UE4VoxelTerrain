// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "SystemInfoWidget.h"


FString USystemInfoWidget::sandboxSystemInfoText() {
	/*
	ALevelController* lc = ALevelController::instance;
	if (lc == NULL) {
		return TEXT("");
	}

	if (lc->zone_queue.Num() > lc->zone_queue_pos) {
		return TEXT("Building terrain");
	}
	*/

	return TEXT("test");
}

float USystemInfoWidget::sandboxSystemInfoPercent() {
	/*
	ALevelController* lc = ALevelController::instance;
	if (lc == NULL) {
		return 0;
	}

	if (lc->zone_queue.Num() > lc->zone_queue_pos) {
		float f = ((float)lc->zone_queue_pos / (float)lc->zone_queue.Num());
		//UE_LOG(LogTemp, Warning, TEXT("%f%%"), f);
		return f;
	}
	*/

	return 0;
}

ESlateVisibility USystemInfoWidget::sandboxSystemInfoVisiblity() {
	/*
	ALevelController* lc = ALevelController::instance;
	if (lc == NULL) {
		return ESlateVisibility::Hidden;
	}

	if (lc->zone_queue.Num() > lc->zone_queue_pos) {
		return ESlateVisibility::Visible;
	}
	

	return ESlateVisibility::Hidden;
	*/

	return ESlateVisibility::Visible;
}

