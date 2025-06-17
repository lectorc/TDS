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
#include "TDS123/WeaponDefault.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TDS123/Character/TDS123InventoryComponent.h"
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

    InventoryComponent = CreateDefaultSubobject<UTDS123InventoryComponent>(TEXT("InventoryComponent"));

    if (InventoryComponent)
    {
        InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATDS123Character::InitWeapon);
    }

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDS123Character::BeginPlay()
{
    Super::BeginPlay();
    if (InventoryComponent && CurrentWeapon)
        InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);

}

void ATDS123Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    MovementTick(DeltaSeconds);

    BlockMovementVector();

    StaminaSystem();

    if (InventoryComponent && CurrentWeapon)
        InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
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
    NewInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATDS123Character::TrySwitchNextWeapon);
    NewInputComponent->BindAction(TEXT("SwitchPreviosWeapon"), EInputEvent::IE_Pressed, this, &ATDS123Character::TrySwitchPreviosWeapon);

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
        ResSpeed = MovementInfo.AimSpeedNormal;
        break;
    case EMovementState::Walk_State:
        ResSpeed = MovementInfo.WalkSpeedNormal;
        break;
    case EMovementState::Run_State:
        ResSpeed = MovementInfo.RunSpeedNormal;
        break;
    case EMovementState::Sprint_State:
        ResSpeed = MovementInfo.SprintSpeedNormal;
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

void ATDS123Character::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
    if (InventoryComponent && CurrentWeapon)
    {
        InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
        InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
    }
    WeaponReloadEnd_BP(bIsSuccess);
}

void ATDS123Character::WeaponFireStart(UAnimMontage* Anim)
{
    if (InventoryComponent && CurrentWeapon)
        InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
    WeaponFireStart_BP(Anim);
}

void ATDS123Character::WeaponFireStart_BP_Implementation(UAnimMontage* Anim)
{
}



void ATDS123Character::TrySwitchNextWeapon()
{
    if (InventoryComponent->WeaponSlots.Num() > 1)
    {
        //We have more then one weapon go switch
        int8 OldIndex = CurrentIndexWeapon;
        FAdditionalWeaponInfo OldInfo;
        if (CurrentWeapon)
        {
            OldInfo = CurrentWeapon->AdditionalWeaponInfo;
            if (CurrentWeapon->WeaponReloading)
                CurrentWeapon->CancelReload();
        }

        if (InventoryComponent)
        {
            if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
            {
            }
        }
    }
}

void ATDS123Character::TrySwitchPreviosWeapon()
{
    if (InventoryComponent->WeaponSlots.Num() > 1)
    {
        //We have more then one weapon go switch
        int8 OldIndex = CurrentIndexWeapon;
        FAdditionalWeaponInfo OldInfo;
        if (CurrentWeapon)
        {
            OldInfo = CurrentWeapon->AdditionalWeaponInfo;
            if (CurrentWeapon->WeaponReloading)
                CurrentWeapon->CancelReload();
        }

        if (InventoryComponent)
        {
            //InventoryComponent->SetAdditionalInfoWeapon(OldIndex, GetCurrentWeapon()->AdditionalWeaponInfo);
            if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
            {
            }
        }
    }
}

void ATDS123Character::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
    //in BP
}

void ATDS123Character::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
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
        UE_LOG(LogTemp, Warning, TEXT("ATDS123Character::AttackCharEvent - CurrentWeapon -NULL"));
}

void ATDS123Character::InitWeapon( FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }

    UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetGameInstance());
    FWeaponInfo myWeaponInfo;
    if (myGI)
    {
        if (myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
        {
            if (myWeaponInfo.WeaponClass)
            {
                FVector SpawnLocation = FVector(0);
                FRotator SpawnRotation = FRotator(0);

                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                SpawnParams.Owner = this;
                SpawnParams.Instigator = GetInstigator();

                AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
                if (myWeapon)
                {
                    FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
                    myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
                    CurrentWeapon = myWeapon;
                    myWeapon->CurrentWeaponIdName = IdWeaponName;
                    myWeapon->WeaponSetting = myWeaponInfo;

                    //myWeapon->AdditionalWeaponInfo.Round = myWeaponInfo.MaxRound;

                        myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
                    myWeapon->UpdateStateWeapon(MovementState);

                    myWeapon->AdditionalWeaponInfo = WeaponAdditionalInfo;
                    //if(InventoryComponent)
                    CurrentIndexWeapon = NewCurrentIndexWeapon;//fix

                    //Not Forget remove delegate on change/drop weapon
                    if (!myWeapon->OnWeaponReloadStart.IsAlreadyBound(this, &ATDS123Character::WeaponReloadStart))
                    myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATDS123Character::WeaponReloadStart);
                    if (!myWeapon->OnWeaponReloadEnd.IsAlreadyBound(this, &ATDS123Character::WeaponReloadEnd))
                    myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATDS123Character::WeaponReloadEnd);
                    if (!myWeapon->OnWeaponFireStart.IsAlreadyBound(this, &ATDS123Character::WeaponFireStart))
                    myWeapon->OnWeaponFireStart.AddDynamic(this, &ATDS123Character::WeaponFireStart);
                    // after switch try reload weapon if needed
                    if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
                        CurrentWeapon->InitReload();

                    if (InventoryComponent)
                        InventoryComponent->OnWeaponAmmoAviable.Broadcast(myWeapon->WeaponSetting.WeaponType);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::InitWeapon - Weapon not found in table -NULL"));
        }
    }
}

void ATDS123Character::TryReloadWeapon()
{
    if (CurrentWeapon)
    {

        if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload())
        {
            CurrentWeapon->InitReload();
        }
    }
}

void ATDS123Character::WeaponReloadStart(UAnimMontage* Anim)
{
        WeaponReloadStart_BP(Anim);
}




AWeaponDefault* ATDS123Character::GetCurrentWeapon()
{
    return CurrentWeapon;
}


