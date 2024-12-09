// Fill out your copyright notice in the Description page of Project Settings.
#include "WeaponDefault.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "TDS123/Character/TDS123InventoryComponent.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    RootComponent = SceneComponent;

    SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
    SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
    SkeletalMeshWeapon->SetupAttachment(RootComponent);

    StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh "));
    StaticMeshWeapon->SetGenerateOverlapEvents(false);
    StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
    StaticMeshWeapon->SetupAttachment(RootComponent);

    ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
    ShootLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
    Super::BeginPlay();

    WeaponInit();

}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FireTick(DeltaTime);
    ReloadTick(DeltaTime);
    DispersionTick(DeltaTime);
    ShellDropTick(DeltaTime);
    ClipDropTick(DeltaTime);
}

void AWeaponDefault::FireTick(float DeltaTime)
{
    if (GetWeaponRound() > 0)
    {
        if (WeaponFiring)
            if (FireTimer < 0.f)
            {

                if (!WeaponReloading)
                    Fire();
            }
            else
                FireTimer -= DeltaTime;
    }
    else
    { 
                if (!WeaponReloading)
                {
                    InitReload();
                }
       
    }
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
    if (WeaponReloading)
    {
        if (ReloadTimer <= 0.0f)
        {
            FinishReload();
        }
        else
        {
            ReloadTimer -= DeltaTime;
        }
    }
}

void AWeaponDefault::ShellDropTick(float DeltaTime)
{
    if (DropShellFlag)
    {
        if (DropShellTimer < 0.0f)
        {
            DropShellFlag = false;
            InitDropMesh(WeaponSetting.ShellBullets.DropMesh, WeaponSetting.ShellBullets.DropMeshOffset, WeaponSetting.ShellBullets.DropMeshImpulseDir, WeaponSetting.ShellBullets.DropMeshLifeTime, WeaponSetting.ShellBullets.ImpulseRandomDispersion, WeaponSetting.ShellBullets.PowerImpulse, WeaponSetting.ShellBullets.CustomMass);
        }
        else
            DropShellTimer -= DeltaTime;
    }
}

void AWeaponDefault::ClipDropTick(float DeltaTime)
{
    if (DropClipFlag)
    {
        if (DropClipTimer < 0.0f)
        {
            DropClipFlag = false;
            InitDropMesh(WeaponSetting.ClipDropMesh.DropMesh, WeaponSetting.ClipDropMesh.DropMeshOffset, WeaponSetting.ClipDropMesh.DropMeshImpulseDir, WeaponSetting.ClipDropMesh.DropMeshLifeTime, WeaponSetting.ClipDropMesh.ImpulseRandomDispersion, WeaponSetting.ClipDropMesh.PowerImpulse, WeaponSetting.ClipDropMesh.CustomMass);
        }
        else
            DropClipTimer -= DeltaTime;
    }
}


void AWeaponDefault::WeaponInit()
{
    if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
    {
        SkeletalMeshWeapon->DestroyComponent(true);
    }

    if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
    {
        StaticMeshWeapon->DestroyComponent();
    }
   
    UpdateStateWeapon(EMovementState::Run_State);
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
    if (CheckWeaponCanFire())
        WeaponFiring = bIsFire;
    else
        WeaponFiring = false;
    FireTimer = 0.01f;
}



bool AWeaponDefault::CheckWeaponCanFire()
{
    return !BlockFire;
}

bool AWeaponDefault::CheckCanWeaponReload()
{
    bool result = true;
    if (GetOwner())
    {
        UTDS123InventoryComponent* MyInv = Cast<UTDS123InventoryComponent>(GetOwner()->GetComponentByClass(UTDS123InventoryComponent::StaticClass()));
        if (MyInv)
        {
            int8 AviableAmmoForWeapon;
            if (!MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
            {
                result = false;
            }
        }
    }
    return result;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
    return WeaponSetting.ProjectileSetting;
}

inline void AWeaponDefault::Fire()
{
    if (WeaponSetting.ShellBullets.DropMesh)
    {
        if (WeaponSetting.ShellBullets.DropMeshTime < 0.0f)
        {
            InitDropMesh(WeaponSetting.ShellBullets.DropMesh, WeaponSetting.ShellBullets.DropMeshOffset, WeaponSetting.ShellBullets.DropMeshImpulseDir, WeaponSetting.ShellBullets.DropMeshLifeTime, WeaponSetting.ShellBullets.ImpulseRandomDispersion, WeaponSetting.ShellBullets.PowerImpulse, WeaponSetting.ShellBullets.CustomMass);
        }
        else
        {
            DropShellFlag = true;
            DropShellTimer = WeaponSetting.ShellBullets.DropMeshTime;
        }
    }

    FireTimer = WeaponSetting.RateOfFire;
    AdditionalWeaponInfo.Round = AdditionalWeaponInfo.Round - 1;
    ChangeDispersionByShot();

    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation-> GetComponentLocation());
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation-> GetComponentTransform());

    int8 NumberProjectile = GetNumberProjectileByShot();

    if (ShootLocation)
    {
        FVector SpawnLocation = ShootLocation->GetComponentLocation();
        FRotator SpawnRotation = ShootLocation->GetComponentRotation();
        FProjectileInfo ProjectileInfo;
        ProjectileInfo = GetProjectile();

        FVector EndLocation;
        for (int8 i = 0; i < NumberProjectile; i++)//Shotgun
        {
            EndLocation = GetFireEndLocation();
            




            if (ProjectileInfo.Projectile)
            {
                //Projectile Init ballistic fire
                FVector Dir = EndLocation - SpawnLocation;
                Dir.Normalize();
                FMatrix MyMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
                SpawnRotation = MyMatrix.Rotator();

                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                SpawnParams.Owner = GetOwner();
                SpawnParams.Instigator = GetInstigator();

                AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
                if (myProjectile)
                {
                    //ToDo Init Projectile settings by id in table row(or keep in weapon table)
                    myProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
                    //Projectile->BulletProjectileMovement->InitialSpeed = 2500.0f;
                }
            }
            else
            {
                FHitResult Hit;
                TArray<AActor*> Actors;
                UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, EndLocation * WeaponSetting.DistanceTrace, ETraceTypeQuery::TraceTypeQuery4, false, Actors, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

                if (ShowDebug)
                    DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + ShootLocation->GetForwardVector() * WeaponSetting.DistanceTrace, FColor::Black, false, 5.f,
                        (uint8)'\000', 0.5f);

                if (Hit.GetActor() && Hit.PhysMaterial.IsValid())
                {
                    EPhysicalSurface mySurfacetype = UGameplayStatics::GetSurfaceType(Hit);
                    if (WeaponSetting.ProjectileSetting.HitDecals.Contains(mySurfacetype))
                    {
                        UMaterialInterface* myMaterial = WeaponSetting.ProjectileSetting.HitDecals[mySurfacetype];
                        if (myMaterial && Hit.GetComponent())
                        {
                            UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), Hit.GetComponent(), NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition);
                        }
                    }
                    if (WeaponSetting.ProjectileSetting.HitFXs.Contains(mySurfacetype))
                    {
                        UParticleSystem* myParticle = WeaponSetting.ProjectileSetting.HitFXs[mySurfacetype];
                        if (myParticle)
                        {
                            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
                        }
                    }

                    if (WeaponSetting.ProjectileSetting.HitSound)
                    {
                        UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSetting.ProjectileSetting.HitSound, Hit.ImpactPoint);
                    }
                    UGameplayStatics::ApplyDamage(Hit.GetActor(), WeaponSetting.ProjectileSetting.ProjectileDamage, GetInstigatorController(), this, NULL);
                }
            }
          }
        }
    if (GetWeaponRound() <= 0 && !WeaponReloading)
    {
        //Init Reload
        if (CheckCanWeaponReload())
            InitReload();
    }
       
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
    //ToDo Dispersion
    BlockFire = false;

    switch (NewMovementState)
    {
    case EMovementState::Aim_State:

        CurrentDispersionMax = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMax;
        CurrentDispersionMin = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMin;
        CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimRecoil;
        CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
        break;
    case EMovementState::AimWalk_State:

        CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMax;
        CurrentDispersionMin = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMin;
        CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimRecoil;
        CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
        break;
    case EMovementState::Walk_State:

        CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMax;
        CurrentDispersionMin = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMin;
        CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimRecoil;
        CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
        break;
    case EMovementState::Run_State:

        CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMax;
        CurrentDispersionMin = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMin;
        CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Run_StateDispersionAimRecoil;
        CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
        break;
    case EMovementState::Sprint_State:
        BlockFire = true;
        SetWeaponStateFire(false);//set fire trigger to false
        //Block Fire
        break;
    default:
        break;
    }
}

void AWeaponDefault::ChangeDispersion()
{
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
    if (!WeaponReloading)
    {
        if (!WeaponFiring)
        {
            if (ShouldReduceDispersion)
                CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
            else
                CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;
        }

        if (CurrentDispersion < CurrentDispersionMin)
        {

            CurrentDispersion = CurrentDispersionMin;

        }
        else
        {
            if (CurrentDispersion > CurrentDispersionMax)
            {
                CurrentDispersion = CurrentDispersionMax;
            }
        }
    }
    if (ShowDebug)
        UE_LOG(LogTemp, Warning, TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
}

void AWeaponDefault::ChangeDispersionByShot()
{
    CurrentDispersion = CurrentDispersion + CurrentDispersionRecoil;
}

FVector AWeaponDefault::GetFireEndLocation() const
{
    bool bShootDirection = false;
    FVector EndLocation = FVector(0.f); 
    FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
    if (tmpV.Size() > SizeVectorToChangeShotDirectionLogic)
    {
        EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
        if(ShowDebug)
        DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSetting.DistanceTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);

    }
    else
    { 
        EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
        if (ShowDebug)
        DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSetting.DistanceTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
    }
    
       
    if (ShowDebug)
    {
        //direction weapon look
        DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 5.f, (uint8)'\000', 0.5f);
        //direction projectile must fly
        DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 5.f, (uint8)'\000', 0.5f);
        //Direction Projectile Current fly
        DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Black, false, 5.f, (uint8)'\000', 0.5f);

        //DrawDebugSphere(GetWorld(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector()*SizeVectorToChangeShootDirectionLogic, 10.f, 8, FColor::Red, false, 4.0f);
    }
    return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShot() const
{
    return WeaponSetting.NumberProjectileByShot;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
    
    return  FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

void AWeaponDefault::BulletLog()
{

}

float AWeaponDefault::GetCurrentDispersion() const
{
    float Result = CurrentDispersion;
    return Result;
}

int32 AWeaponDefault::GetWeaponRound()
{
    return AdditionalWeaponInfo.Round;
}

void AWeaponDefault::InitReload()
{
    if (WeaponReloading == false)
    { 
    WeaponReloading = true;

    ReloadTimer = WeaponSetting.ReloadTime;

    UAnimMontage* AnimToPlay = nullptr;
        AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharReload;

    OnWeaponReloadStart.Broadcast(AnimToPlay);

    UAnimMontage* AnimWeaponToPlay = nullptr;

        AnimWeaponToPlay = WeaponSetting.AnimWeaponInfo.AnimWeaponReload;

    if (WeaponSetting.AnimWeaponInfo.AnimWeaponReload
        && SkeletalMeshWeapon
        && SkeletalMeshWeapon->GetAnimInstance())
    {
        SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(AnimWeaponToPlay);
    }

    if (WeaponSetting.ClipDropMesh.DropMesh)
    {
        DropClipFlag = true;
        DropClipTimer = WeaponSetting.ClipDropMesh.DropMeshTime;
    }
    }
}

void AWeaponDefault::FinishReload()
{
    WeaponReloading = false;
    AdditionalWeaponInfo.Round = WeaponSetting.MaxRound;
        
    OnWeaponReloadEnd.Broadcast(true);
}

void AWeaponDefault::InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float LifeTimeMesh, float ImpulseRandomDispersion, float PowerImpulse, float CustomMass)
{
    if (DropMesh)
    {
        FTransform Transform;

        FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() * Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;

        Transform.SetLocation(GetActorLocation() + LocalDir);
        Transform.SetScale3D(Offset.GetScale3D());

        Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());
        AStaticMeshActor* NewActor = nullptr;

        FActorSpawnParameters Param;
        Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        Param.Owner = this;
        NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Param);

        if (NewActor && NewActor->GetStaticMeshComponent())
        {
            NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
            NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

            NewActor->SetActorTickEnabled(false);
            NewActor->InitialLifeSpan = LifeTimeMesh;

            NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
            NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
            NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);

            NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECollisionResponse::ECR_Ignore);
            NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel12, ECollisionResponse::ECR_Ignore);
            NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
            NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
            NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
            NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);

            if (CustomMass > 0.0f)
            {
                NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, CustomMass, true);
            }

            if (!DropImpulseDirection.IsNearlyZero())
            {
                FVector FinalDir;
                LocalDir = LocalDir + (DropImpulseDirection * 1000.0f);

                if (!FMath::IsNearlyZero(ImpulseRandomDispersion))
                    FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandomDispersion);
                FinalDir.GetSafeNormal(0.0001f);

                NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir * PowerImpulse);
            }
        }
    }
}

void AWeaponDefault::CancelReload()
{
    WeaponReloading = false;
    if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
        SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);

    OnWeaponReloadEnd.Broadcast(false);
    DropClipFlag = false;
}
