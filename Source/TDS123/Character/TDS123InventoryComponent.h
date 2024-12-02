// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDS123/FuncLibrary/UType.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "TDS123/Game/TDS123GameInstance.h"
#include "TDS123InventoryComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChange, int32, IndexSlot, FAdditionalWeaponInfo, AdditionalInfo);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TDS123_API UTDS123InventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UTDS123InventoryComponent();

    FOnSwitchWeapon OnSwitchWeapon;
    

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    TArray<FWeaponSlot> WeaponSlots;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    TArray<FAmmoSlot> AmmoSlots;
    UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FOnWeaponAdditionalInfoChange OnWeaponAdditionalInfoChange;

    int32 MaxSlotsWeapon = 0;

    FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
    int32 GetWeaponIndexSlotByName(FName IdWeaponName);
    void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

};
