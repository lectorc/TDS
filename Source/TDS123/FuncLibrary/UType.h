// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UType.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
    

    Aim_State UMETA(DisplayName = "Aim State"),
    Walk_State UMETA(DisplayName = "Walk State"),
    Run_State UMETA(DisplayName = "Run State"),
    Sprint_State UMETA(DisplayName = "Sprint State")
};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float AimSpeed = 300.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float WalkSpeed = 200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float RunSpeed = 500.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float SprintSpeed = 750.0f;
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    TSubclassOf<class AProjectileDefault> Projectile = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    float ProjectileDamage = 20.0f;
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSetting")
    float WeaponDamage = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSetting")
    float RateOfFire = 0.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSetting")
    FProjectileInfo ProjectileSetting;
};

UCLASS()
class TDS123_API UTypes : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
};
