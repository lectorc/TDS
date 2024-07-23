// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDS123Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

ATDS123Character::ATDS123Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDS123Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    MovementTick(DeltaSeconds);
}

    void ATDS123Character::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    Super::SetupPlayerInputComponent(NewInputComponent);

    NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATDS123Character::InputAxisX);
    NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATDS123Character::InputAxisY);

}

void ATDS123Character::InputAxisX(float Value)
{
    AxisX = Value;
}

void ATDS123Character::InputAxisY(float Value)
{
    AxisY = Value;
}

void ATDS123Character::MovementTick(float DeltaTime)
{
    
    AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
    AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

    APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (myController)
    {
        FHitResult ResultHit;
        myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
        float FindRotaterResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
        SetActorRotation(FQuat(FRotator(0.0f, FindRotaterResultYaw, 0.0f)));
    }
    
    
}

void ATDS123Character::CharacterUpdate()
{
    float ResSpeed = 600.0f;
    switch (MovementState)
    {
    case EMovementState::Aim_State:
        ResSpeed = MovementInfo.AimSpeed;
        break;
    case EMovementState::Walk_State:
        ResSpeed = MovementInfo.WalkSpeed;
        break;
    case EMovementState::Run_State:
        ResSpeed = MovementInfo.RunSpeed;
        break;
    default:
        break;
    }

    GetCharacterMovement()->MaxWalkSpeed = ResSpeed;    
}

void ATDS123Character::ChangeMovementState(EMovementState NewMovementState)
{
    MovementState = NewMovementState;
    CharacterUpdate();
}