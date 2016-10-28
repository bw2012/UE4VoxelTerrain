// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4VoxelTerrain.h"
#include "SandboxCharacter.h"


// Sets default values
ASandboxCharacter::ASandboxCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASandboxCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASandboxCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ASandboxCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("ZoomIn", IE_Released, this, &ASandboxCharacter::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Released, this, &ASandboxCharacter::ZoomOut);


	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &ASandboxCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASandboxCharacter::MoveRight);


	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &ASandboxCharacter::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ASandboxCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &ASandboxCharacter::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ASandboxCharacter::LookUpAtRate);

}

void ASandboxCharacter::ZoomIn() {
	if (GetCameraBoom() == NULL) return;
	if (GetCameraBoom()->TargetArmLength > 200) {
		GetCameraBoom()->TargetArmLength -= 100;
	}

	UE_LOG(LogTemp, Warning, TEXT("ZoomIn: %f"), GetCameraBoom()->TargetArmLength);
}

void ASandboxCharacter::ZoomOut() {
	if (GetCameraBoom() == NULL) return;
	if (GetCameraBoom()->TargetArmLength < 2500) {
		GetCameraBoom()->TargetArmLength += 100;
	}

	UE_LOG(LogTemp, Warning, TEXT("ZoomOut: %f"), GetCameraBoom()->TargetArmLength);
}

void ASandboxCharacter::initTopDownView() {

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does

	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	CameraBoom->ProbeSize = 0;
	CameraBoom->RelativeLocation = FVector(0, 0, 0);

	//FollowCamera->DetachFromParent();
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->RelativeLocation = FVector(0, 0, 0); // Position the camera
}

void ASandboxCharacter::initThirdPersonView() {
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeSize = 12;
	CameraBoom->RelativeLocation = FVector(40, 30, 64);

	FollowCamera->DetachFromParent();
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->RelativeLocation = FVector(0, 0, 0); // Position the camera
}


void ASandboxCharacter::AddControllerYawInput(float Val) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }
	if (view == PlayerView::TOP_DOWN) return;

	Super::AddControllerYawInput(Val);

}

void ASandboxCharacter::AddControllerPitchInput(float Val) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }
	if (view == PlayerView::TOP_DOWN) return;

	Super::AddControllerPitchInput(Val);
}

void ASandboxCharacter::TurnAtRate(float Rate) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }
	if (view == PlayerView::TOP_DOWN) return;

	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASandboxCharacter::LookUpAtRate(float Rate) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }
	if (view == PlayerView::TOP_DOWN) return;

	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ASandboxCharacter::MoveForward(float Value) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }

	if (view == PlayerView::THIRD_PERSON) {
		if (Value != 0.0f)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}

	if (view == PlayerView::FIRST_PERSON) {
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

void ASandboxCharacter::MoveRight(float Value) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }

	if (view == PlayerView::THIRD_PERSON) {
		if (Value != 0.0f)
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}

	if (view == PlayerView::FIRST_PERSON) {
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}