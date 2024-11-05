// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDS123Character.h"
#include "TDS123/Game/TDS123GameInstance.h"
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

void ATDS123Character::BeginPlay()
{
    Super::BeginPlay();

    InitWeapon(InitWeaponName);

}

void ATDS123Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    MovementTick(DeltaSeconds);

    BlockMovementVector();

    StaminaSystem();

}



    void ATDS123Character::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    Super::SetupPlayerInputComponent(NewInputComponent);

    NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATDS123Character::InputAxisX);
    NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATDS123Character::InputAxisY);
    NewInputComponent->BindAction("Sprint", IE_Pressed, this, &ATDS123Character::ChangeMovementStateToSprint);
    NewInputComponent->BindAction("Sprint", IE_Released, this, &ATDS123Character::ChangeMovementStateToRun);
    //NewInputComponent->BindAction("Walk", IE_Pressed, this, &ATDS123Character::ChangeMovementStateToWalk);
   // NewInputComponent->BindAction("Walk", IE_Released, this, &ATDS123Character::ChangeMovementStateToRun);
    NewInputComponent->BindAction("Aim", IE_Pressed, this, &ATDS123Character::ChangeMovementStateToAim);
    NewInputComponent->BindAction("Aim", IE_Released, this, &ATDS123Character::ChangeMovementStateToRun);
    NewInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ATDS123Character::InputAttackPressed);
    NewInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &ATDS123Character::InputAttackReleased);
    NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &ATDS123Character::TryReloadWeapon);
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
        myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
        float FindRotaterResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
        SetActorRotation(FQuat(FRotator(0.0f, FindRotaterResultYaw, 0.0f)));

        if (CurrentWeapon)
        {
            FVector Displacement = FVector(0);
            switch (MovementState)
            {
            case EMovementState::Aim_State:
                Displacement = FVector(0.0f, 0.0f, 160.0f);
                CurrentWeapon->ShouldReduceDispersion = true;
                break;
            case EMovementState::Walk_State:
                Displacement = FVector(0.0f, 0.0f, 120.0f);
                CurrentWeapon->ShouldReduceDispersion = false;
                break;
            case EMovementState::Run_State:
                Displacement = FVector(0.0f, 0.0f, 120.0f);
                CurrentWeapon->ShouldReduceDispersion = false;
                break;
            case EMovementState::Sprint_State:
                Displacement = FVector(0.0f, 0.0f, 120.0f);
                break;
            default:
                break;
            }
            CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
        }
    }
    
    if (CurrentWeapon)
        if (FMath::IsNearlyZero(GetVelocity().Size(), 0.5f))
            CurrentWeapon->ShouldReduceDispersion = true;
        else
            CurrentWeapon->ShouldReduceDispersion = false;
    
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
    case EMovementState::Sprint_State:
        ResSpeed = MovementInfo.SprintSpeed;
        AddMovementInput(FVector(0.0f, 0.0f, 0.0f), AxisY);
    default:
        break;
    }

    GetCharacterMovement()->MaxWalkSpeed = ResSpeed;    
}

void ATDS123Character::InputAttackPressed()
{
    AttackCharEvent(true);
}



void ATDS123Character::InputAttackReleased()
{
    AttackCharEvent(false);
}

void ATDS123Character::ChangeBlockedSprint()
{

}

void ATDS123Character::BlockMovementVector()
{
    if (SprintEnabled == true)
    {
        FVector InputCharacterMovement;
        FVector ForwardVector;
        ForwardVector = GetActorForwardVector();
        InputCharacterMovement = GetCharacterMovement()->GetLastInputVector();
        float DotProductResult = FVector::DotProduct(InputCharacterMovement.GetSafeNormal(), ForwardVector.GetSafeNormal());
        float AngleInRadians = FMath::Acos(DotProductResult);
        float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);
        if (AngleInDegrees > 30 || Stamina < 0 )
        {
            MovementState = EMovementState::Run_State;
            SprintEnabled = false;


        }
        
        CharacterUpdate();
    }
}

void ATDS123Character::StaminaSystem()
{
    if (!SprintEnabled && Stamina < MaxStamina)
    {
        Stamina = FMath::Clamp(Stamina + 0.001, -0.05, MaxStamina);
    }

    if (!SprintEnabled) return;

    if (Stamina > 0.0f && SprintEnabled)
    {
        Stamina = FMath::Clamp(Stamina - 0.005, -0.05, MaxStamina);
    }
   
}

void ATDS123Character::ChangeMovementStateToSprint()
{
    MovementState = EMovementState::Sprint_State;
    SprintEnabled = true;
}

void ATDS123Character::ChangeMovementStateToRun()
{
    
    MovementState = EMovementState::Run_State;
    AimEnabled = false;

}

void ATDS123Character::ChangeMovementStateToWalk()
{
    MovementState = EMovementState::Walk_State;
    AimEnabled = false;
    
}

void ATDS123Character::WeaponReloadStart(UAnimMontage* Anim)
{
    WeaponReloadStart_BP(Anim);
}

void ATDS123Character::WeaponReloadEnd()
{
    WeaponReloadEnd_BP();
}

void ATDS123Character::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
    //in BP
}

void ATDS123Character::WeaponReloadEnd_BP_Implementation()
{
    //in BP
}


void ATDS123Character::ChangeMovementStateToAim()
{
    MovementState = EMovementState::Aim_State;
    AimEnabled = true;
}

void ATDS123Character::ChangeMovementState()
{
    if (!WalkEnabled && !SprintRunEnabled && !AimEnabled)
    {
        MovementState = EMovementState::Run_State;
    }
    else
    {
        if (SprintRunEnabled)
        {
            WalkEnabled = false;
            AimEnabled = false;
            MovementState = EMovementState::SprintRun_State;
        }
        if (WalkEnabled && !SprintRunEnabled && AimEnabled)
        {
            MovementState = EMovementState::AimWalk_State;
        }
        else
        {
            if (WalkEnabled && !SprintRunEnabled && !AimEnabled)
            {
                MovementState = EMovementState::Walk_State;
            }
            else
            {
                if (!WalkEnabled && !SprintRunEnabled && AimEnabled)
                {
                    MovementState = EMovementState::Aim_State;
                }
            }
        }
    }
    CharacterUpdate();

    //Weapon state update
    AWeaponDefault* myWeapon = GetCurrentWeapon();
    if (myWeapon)
    {
        myWeapon->UpdateStateWeapon(MovementState);
    }
}



void ATDS123Character::InitWeapon(FName IdWeapon)
{
    UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetGameInstance());
    FWeaponInfo myWeaponInfo;
    if (myGI)
    {
        if (myGI->GetWeaponInfoByName(IdWeapon, myWeaponInfo))
        {
            if (myWeaponInfo.WeaponClass)
            {
                FVector SpawnLocation = FVector(0);
                FRotator SpawnRotation = FRotator(0);

                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                SpawnParams.Owner = GetOwner();
                SpawnParams.Instigator = GetInstigator();

                AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
                if (myWeapon)
                {
                    FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
                    myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
                    CurrentWeapon = myWeapon;
                    myWeapon->WeaponSetting = myWeaponInfo;
                    myWeapon->UpdateStateWeapon(MovementState);
                    myWeapon->WeaponInfo.Round = myWeaponInfo.MaxRound;
                    myWeapon->ReloadTime = myWeaponInfo.ReloadTime;

                    if(!myWeapon->OnWeaponReloadStart.IsAlreadyBound(this, &ATDS123Character::WeaponReloadStart))
                    myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATDS123Character::WeaponReloadStart);
                    if (!myWeapon->OnWeaponReloadEnd.IsAlreadyBound(this, &ATDS123Character::WeaponReloadEnd))
                    myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATDS123Character::WeaponReloadEnd);
                }
            }
        }
        
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ATDS123Character::InitWeapon - Weapon not found in table -NULL"));
    }
}

void ATDS123Character::AttackCharEvent(bool bIsFiring)
{
    AWeaponDefault* myWeapon = nullptr;
    myWeapon = GetCurrentWeapon();
    if (myWeapon)
    {
        //ToDo Check melee or range
        myWeapon->SetWeaponStateFire(bIsFiring);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
}

void ATDS123Character::TryReloadWeapon()
{
    if (CurrentWeapon)
    {

        if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound)
        {
            CurrentWeapon->InitReload();
        }
    }
}



AWeaponDefault* ATDS123Character::GetCurrentWeapon()
{
    return CurrentWeapon;
}


