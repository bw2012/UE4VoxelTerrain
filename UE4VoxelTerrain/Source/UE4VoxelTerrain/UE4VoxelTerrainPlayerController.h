// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "SandboxPlayerController.h"
#include "UE4VoxelTerrainPlayerController.generated.h"


UCLASS()
class AUE4VoxelTerrainPlayerController : public ASandboxPlayerController
{
	GENERATED_BODY()

public:
	AUE4VoxelTerrainPlayerController();

protected:

	virtual void PlayerTick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	virtual void OnMainActionPressed();

	virtual void OnMainActionReleased();

	virtual void OnAltActionPressed();

	virtual void OnAltActionReleased();

	void PerformAction();

public:

	int tool_mode = 0;

	FHitResult TracePlayerActionPoint();

private:

	FTimerHandle timer;

	void setTool0();
	
	void setTool1();

	void setTool2();

	void setTool3();

	void setTool4();

};


