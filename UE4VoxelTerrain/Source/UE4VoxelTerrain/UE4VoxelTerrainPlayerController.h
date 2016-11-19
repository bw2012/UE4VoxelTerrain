// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "UE4VoxelTerrainPlayerController.generated.h"


UCLASS()
class AUE4VoxelTerrainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AUE4VoxelTerrainPlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	// ==========================================================================================================

public:
	int tool_mode = 0;

private:
	void OnMainActionPressed();

	void OnMainActionReleased();

	FHitResult CerrentPos;
	
	void setTool1();

	void setTool2();

	void ToggleView();

	FTimerHandle timer;

	void PerformAction();

};


