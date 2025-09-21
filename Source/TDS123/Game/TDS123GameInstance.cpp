
#include "TDS123GameInstance.h"

bool UTDS123GameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo)
{
    bool bIsFind = false;
    FWeaponInfo* WeaponInfoRow;

    if (WeaponInfoTable)
    {
        WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
        if (WeaponInfoRow)
        {
            bIsFind = true;
            OutInfo = *WeaponInfoRow;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UTPSGameInstance::GetWeaponInfoByName - WeaponTable -NULL"));
    }

    return bIsFind;
}

bool UTDS123GameInstance::GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo)
{
    bool bIsFind = false;

    if (DropItemInfoTable)
    {
        FDropItem* DropItemInfoRow;
        TArray<FName>RowNames = DropItemInfoTable->GetRowNames();

        int8 i = 0;
        while (i < RowNames.Num() && !bIsFind)
        {
            DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowNames[i], "");
            if (DropItemInfoRow->WeaponInfo.NameItem == NameItem)
            {
                OutInfo = (*DropItemInfoRow);
                bIsFind = true;
            }
            i++;//fix
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UTPSGameInstance::GetDropItemInfoByName - DropItemInfoTable -NULL"));
    }

    return bIsFind;
}

bool UTDS123GameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo)
{
    bool bIsFind = false;
    FDropItem* DropItemInfoRow;

    if (DropItemInfoTable)
    {
        DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(NameItem, "", false);
        if (DropItemInfoRow)
        {
            bIsFind = true;
            OutInfo = *DropItemInfoRow;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UTPSGameInstance::GetWeaponInfoByName - DropItemInfoTable -NULL"))
    }

    return bIsFind;
}

