// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "SystemInfoWidget.h"
#include "TerrainController.h"


ATerrainController* USystemInfoWidget::getController() {
	if (controller == NULL || !controller->IsValidLowLevel()) {
		for (TActorIterator<ATerrainController> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			controller = *ActorItr;
			break;
		}
	}

	return controller;
}

FString USystemInfoWidget::sandboxSystemInfoText() {
	//AMainHUD* hud = (AMainHUD*)GetOwningPlayer()->GetHUD();

	if (getController() != NULL) {
		if (getController()->ZoneLoaderConter < getController()->ZoneLoaderTotal) {
			return TEXT("Building terrain");
		}
	}

	return TEXT("");
}

float USystemInfoWidget::sandboxSystemInfoPercent() {
	if (getController() != NULL) {
		if (getController()->ZoneLoaderConter < getController()->ZoneLoaderTotal) {
			float f = ((float)getController()->ZoneLoaderConter / (float)getController()->ZoneLoaderTotal);
			return f;
		}
	}

	return 0;
}

ESlateVisibility USystemInfoWidget::sandboxSystemInfoVisiblity() {
	if (getController() != NULL) {
		if (getController()->ZoneLoaderConter < getController()->ZoneLoaderTotal) {
			return ESlateVisibility::Visible;
		}
	}

	return ESlateVisibility::Hidden;
}

