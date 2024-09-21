// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"

#include "FuncLibrary/UType.h"
#include "ProjectileDefault.h"
#include "WeaponDefault.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStart);//ToDo Delegate on event weapon fire - Anim char, state char...

UCLASS()
class TDS123_API AWeaponDefault : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AWeaponDefault();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class USceneComponent* SceneComponent = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class UStaticMeshComponent* StaticMeshWeapon = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class UArrowComponent* ShootLocation = nullptr;

    UPROPERTY()
    FWeaponInfo WeaponSetting;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
    FAddicionalWeaponInfo WeaponInfo;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Tick func
    virtual void Tick(float DeltaTime) override;

    void FireTick(float DeltaTime);

    void WeaponInit();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
    bool WeaponFiring = false;

    UFUNCTION(BlueprintCallable)
    void SetWeaponStateFire(bool bIsFire);

    bool CheckWeaponCanFire();

    FProjectileInfo GetProjectile();

    void Fire();

    void UpdateStateWeapon(EMovementState NewMovementState);
    void ChangeDispersion();


    UFUNCTION()
    void BulletLog();

    //Timers'flags
    float FireTime = 0.0;
};
