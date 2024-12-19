#include "TDS123InventoryComponent.h"
#include "TDS123InventoryComponent.h"
#include "TDS123InventoryComponent.h"
#include "TDS123InventoryComponent.h"
#include "TDS123InventoryComponent.h"
#include "TDS123InventoryComponent.h"
#include "TDS123InventoryComponent.h"
#include "TDS123InventoryComponent.h"
#include "TDS123InventoryComponent.h"
#include "TDS123/Game/TDS123GameInstance.h"
#pragma optimize ("", off)

// Sets default values
UTDS123InventoryComponent::UTDS123InventoryComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;

}

FName UTDS123InventoryComponent::GetWeaponNameBySlotIndex(int32 indexSlot)
{
    FName result;

    if (WeaponSlots.IsValidIndex(indexSlot))
    {
        result = WeaponSlots[indexSlot].NameItem;
    }
    return result;
}

// Called when the game starts or when spawned
void UTDS123InventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    for (int8 i = 0; i < WeaponSlots.Num(); i++)
    {
        UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());
        if (myGI)
        {
            if (!WeaponSlots[i].NameItem.IsNone())
            {
                FWeaponInfo Info;
                if (myGI->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
                    WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;
                else
                {
                    WeaponSlots.RemoveAt(i);
                    i--;
                }
            }
        }
    }

    MaxSlotsWeapon = WeaponSlots.Num();

    if (WeaponSlots.IsValidIndex(0))
    {
        if (!WeaponSlots[0].NameItem.IsNone())
            OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
    }
	
}



void UTDS123InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UTDS123InventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
    bool bIsSuccess = false;
    int8 CorrectIndex = ChangeToIndex;
    if (ChangeToIndex > WeaponSlots.Num() - 1)
        CorrectIndex = 0;
    else
        if (ChangeToIndex < 0)
            CorrectIndex = WeaponSlots.Num() - 1;

    FName NewIdWeapon;
    FAdditionalWeaponInfo NewAdditionalInfo;
    int32 NewCurrentIndex = 0;

    int8 i = 0;
    while (i < WeaponSlots.Num() && !bIsSuccess)
    {
        if (WeaponSlots[i].IndexSlot == CorrectIndex)
        {
            if (!WeaponSlots[i].NameItem.IsNone())
            {
                NewIdWeapon = WeaponSlots[i].NameItem;
                NewAdditionalInfo = WeaponSlots[i].AdditionalInfo;
                bIsSuccess = true;

            }
        }
        i++;
    }

    if (!bIsSuccess)
    {
        
    }

    if (bIsSuccess)
    {
        SetAdditionalInfoWeapon(OldIndex, OldInfo);
        OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
        //OnWeaponAmmoAviable.Broadcast()
    }


    return bIsSuccess;
}

bool UTDS123InventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AviableAmmoForWeapon)
{
    AviableAmmoForWeapon = 0;
    bool bIsFind = false;
    int8 i = 0;
    while (i < AmmoSlots.Num() && !bIsFind)
    {
        if (AmmoSlots[i].WeaponType == TypeWeapon)
        {
            bIsFind = true;
            AviableAmmoForWeapon = AmmoSlots[i].Cout;
            if (AmmoSlots[i].Cout > 0)
            {
                //OnWeaponAmmoAviable.Broadcast(TypeWeapon);//remove not here, only when pickUp ammo this type, or swithc weapon
                return true;
            }

        }

        i++;
    }

    OnWeaponAmmoEmpty.Broadcast(TypeWeapon);//visual empty ammo slot

    return false;
}

bool UTDS123InventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
    bool result = false;
    int8 i = 0;
    while (i < AmmoSlots.Num() && !result)
    {
        if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Cout < AmmoSlots[i].MaxCout)
            result = true;
        i++;
    }
    return result;
}

bool UTDS123InventoryComponent::CheckCanTakeWeapon(int32& FreeSlot)
{
    bool bIsFreeSlot = false;
    int8 i = 0;
    while (i < WeaponSlots.Num() && !bIsFreeSlot)
    {
        if (WeaponSlots[i].NameItem.IsNone())
        {
            bIsFreeSlot = true;
            FreeSlot = i;
        }
        i++;
    }
    return bIsFreeSlot;
}

bool UTDS123InventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo)
{
    bool result = false;

    if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
    {
        WeaponSlots[IndexSlot] = NewWeapon;

        SwitchWeaponToIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);

        OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);
        result = true;
    }
    return result;
}

bool UTDS123InventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
    int32 indexSlot = -1;
    if (CheckCanTakeWeapon(indexSlot))
    {
        if (WeaponSlots.IsValidIndex(indexSlot))
        {
            WeaponSlots[indexSlot] = NewWeapon;

            OnUpdateWeaponSlots.Broadcast(indexSlot, NewWeapon);
            return true;
        }
    }
    return false;
}

bool UTDS123InventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
    bool result = false;

    FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

    UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());
    if (myGI)
    {
        result = myGI->GetDropItemInfoByName(DropItemName, DropItemInfo);
        if (WeaponSlots.IsValidIndex(IndexSlot))
        {
            DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlots[IndexSlot].AdditionalInfo;
        }
    }

    return result;
}

FAdditionalWeaponInfo UTDS123InventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
    FAdditionalWeaponInfo result;
    if (WeaponSlots.IsValidIndex(IndexWeapon))
    {
        bool bIsFind = false;
        int8 i = 0;
        while (i < WeaponSlots.Num() && !bIsFind)
        {
            if (i == IndexWeapon)
            {
                result = WeaponSlots[i].AdditionalInfo;
                bIsFind = true;
                OnAmmoChange.Broadcast(EWeaponType::RifleType, 32);
            }
            i++;
        }
        if (!bIsFind)
            UE_LOG(LogTemp, Warning, TEXT("UTDS123InventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("UTDS123InventoryComponent::SetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);

    return result;
}

int32 UTDS123InventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
    int32 result = -1;
    int8 i = 0;
    bool bIsFind = false;
    while (i < WeaponSlots.Num() && !bIsFind)
    {
        if (WeaponSlots[i].NameItem == IdWeaponName)
        {
            bIsFind = true;
            result = i;
        }
        i++;
    }
    return result;
}

void UTDS123InventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
    if (WeaponSlots.IsValidIndex(IndexWeapon))
    {
        bool bIsFind = false;
        int8 i = 0;
        while (i < WeaponSlots.Num() && !bIsFind)
        {
            if (i == IndexWeapon)
            {
                WeaponSlots[i].AdditionalInfo = NewInfo;
                bIsFind = true;

               
            }
            i++;
        }
        if (!bIsFind)
            UE_LOG(LogTemp, Warning, TEXT("UTDS123InventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("UTDS123InventoryComponent::SetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);
}

void UTDS123InventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo)
{
    bool bIsFind = false;
    int8 i = 0;
    while (i < AmmoSlots.Num() && !bIsFind)
    {
        if (AmmoSlots[i].WeaponType == TypeWeapon)
        {
            AmmoSlots[i].Cout += CoutChangeAmmo;
            if (AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
                AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;

            OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Cout);

            bIsFind = true;
        }
        i++;
    }
}



