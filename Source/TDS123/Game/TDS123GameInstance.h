// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TDS123/FuncLibrary/UType.h"
#include "Engine/DataTable.h"
#include "TDS123/WeaponDefault.h"
#include "TDS123GameInstance.generated.h"

/**
 *
 */
UCLASS()
class TDS123_API UTDS123GameInstance : public UGameInstance
{
    GENERATED_BODY()


public:
    //table
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
    //UFUNCTION(BlueprintCallable)
    //bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo);
};
