// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4VoxelTerrain.h"
#include "UE4VoxelTerrainPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SandboxTerrainController.h"
#include "SandboxCharacter.h"

AUE4VoxelTerrainPlayerController::AUE4VoxelTerrainPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	tool_mode = 1;
}

void AUE4VoxelTerrainPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AUE4VoxelTerrainPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AUE4VoxelTerrainPlayerController::OnSetDestinationReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("Test", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::OnMainActionPressed);
	InputComponent->BindAction("Test", IE_Released, this, &AUE4VoxelTerrainPlayerController::OnMainActionReleased);

	InputComponent->BindAction("1", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool1);
	InputComponent->BindAction("2", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::setTool2);
	InputComponent->BindAction("ToggleView", IE_Pressed, this, &AUE4VoxelTerrainPlayerController::ToggleView);
}

void AUE4VoxelTerrainPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void AUE4VoxelTerrainPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AUE4VoxelTerrainPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AUE4VoxelTerrainPlayerController::OnSetDestinationPressed()
{

	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->view != PlayerView::TOP_DOWN) {
		return;
	}

	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AUE4VoxelTerrainPlayerController::OnSetDestinationReleased()
{
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->view != PlayerView::TOP_DOWN) {
		return;
	}

	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}


void AUE4VoxelTerrainPlayerController::OnMainActionReleased() {
	GetWorld()->GetTimerManager().ClearTimer(timer);
}


void AUE4VoxelTerrainPlayerController::OnMainActionPressed()
{
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->view != PlayerView::TOP_DOWN) {
		return;
	}

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	if (Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("test point -> %f %f %f"), Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z);
		
		// TODO: check controller
		ASandboxTerrainController* controller = ASandboxTerrainController::GetZoneInstance(Hit.Actor.Get());
		if (controller != NULL) {
			if (tool_mode == 1) {
				controller->digTerrainRoundHole(Hit.ImpactPoint, 80, 5);
				GetWorld()->GetTimerManager().SetTimer(timer, this, &AUE4VoxelTerrainPlayerController::PerformAction, 0.2, true);
			}

			if (tool_mode == 2) {
				controller->digTerrainCubeHole(Hit.ImpactPoint, 110, 5);
			}
		}
	}
}

void AUE4VoxelTerrainPlayerController::setTool1()
{
	tool_mode = 1;
}

void AUE4VoxelTerrainPlayerController::setTool2()
{
	tool_mode = 2;
}

void AUE4VoxelTerrainPlayerController::ToggleView() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());

	if (pawn->view == PlayerView::TOP_DOWN) {
		pawn->initThirdPersonView();
		bShowMouseCursor = false;
		pawn->view = PlayerView::THIRD_PERSON;
		//hud->openTpvHud();
		//disableZCut();
	} else if (pawn->view == PlayerView::THIRD_PERSON) {
		pawn->initTopDownView();
		bShowMouseCursor = true;
		pawn->view = PlayerView::TOP_DOWN;

		//hud->closeTpvHud();
		//z_cut_context.force_check = true;
	} 
}

void AUE4VoxelTerrainPlayerController::PerformAction()
{
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->view != PlayerView::TOP_DOWN) {
		return;
	}

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	if (Hit.bBlockingHit) {
		ASandboxTerrainController* controller = ASandboxTerrainController::GetZoneInstance(Hit.Actor.Get());
		if (controller != NULL) {
			if (tool_mode == 1) {
				controller->digTerrainRoundHole(Hit.ImpactPoint, 80, 5);
			}
		}
	}
}




