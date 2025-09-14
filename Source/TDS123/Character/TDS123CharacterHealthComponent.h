// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDS123/Character/TDS123_HealthComponent.h"
#include "TDS123CharacterHealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChange, float, Shield, float, Damage);
/**
 * 
 */
UCLASS()
class TDS123_API UTDS123CharacterHealthComponent : public UTDS123_HealthComponent
{
	GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health ")
    FOnShieldChange OnShieldChange;

    FTimerHandle TimerHandle_CollDownShieldTimer;
    FTimerHandle TimerHandle_ShieldRecoveryRateTimer;


protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float Shield = 400.0f;

public:

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float ShieldRecover = 10.0f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float CoolDownShieldRecoveryTime = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float ShieldRecoverValue = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float ShieldRecoverRate = 0.1f;

    void ChangeHealthValue(float ChangeValue) override;

    float GetCurrentShield();

    void ChangeShieldValue(float ChangeValue);

    void CoolDownShieldEnd();

    void RecoveryShield();
};
