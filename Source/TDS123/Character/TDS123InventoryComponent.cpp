
#include "TDS123InventoryComponent.h"
#include "TDS123/Game/TDS123GameInstance.h"

// Sets default values
UTDS123InventoryComponent::UTDS123InventoryComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
                    //WeaponSlots.RemoveAt(i);
                    //i--;
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

    if (WeaponSlots.IsValidIndex(CorrectIndex))
    {
        if (!WeaponSlots[CorrectIndex].NameItem.IsNone())
        {
            if (WeaponSlots[CorrectIndex].AdditionalInfo.Round > 0)
            {
                //good weapon have ammo start change
                bIsSuccess = true;
            }
            else
            {
                UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());
                if (myGI)
                {
                    //check ammoSlots for this weapon
                    FWeaponInfo myInfo;
                    myGI->GetWeaponInfoByName(WeaponSlots[CorrectIndex].NameItem, myInfo);

                    bool bIsFind = false;
                    int8 j = 0;
                    while (j < AmmoSlots.Num() && !bIsFind)
                    {
                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                        {
                            //good weapon have ammo start change
                            bIsSuccess = true;
                            bIsFind = true;
                        }
                        j++;
                    }
                }
            }
            if (bIsSuccess)
            {
                NewCurrentIndex = CorrectIndex;
                NewIdWeapon = WeaponSlots[CorrectIndex].NameItem;
                NewAdditionalInfo = WeaponSlots[CorrectIndex].AdditionalInfo;
            }
        }
    }

    if (!bIsSuccess)
    {
        if (bIsForward)
        {
            int8 iteration = 0;
            int8 Seconditeration = 0;
            while (iteration < WeaponSlots.Num() && !bIsSuccess)
            {
                iteration++;
                int8 tmpIndex = ChangeToIndex + iteration;
                if (WeaponSlots.IsValidIndex(tmpIndex))
                {
                    if (!WeaponSlots[tmpIndex].NameItem.IsNone())
                    {
                        if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
                        {
                            //WeaponGood
                            bIsSuccess = true;
                            NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
                            NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
                            NewCurrentIndex = tmpIndex;
                        }
                        else
                        {
                            FWeaponInfo myInfo;
                            UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());

                            myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

                            bool bIsFind = false;
                            int8 j = 0;
                            while (j < AmmoSlots.Num() && !bIsFind)
                            {
                                if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                                {
                                    //WeaponGood
                                    bIsSuccess = true;
                                    NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
                                    NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
                                    NewCurrentIndex = tmpIndex;
                                    bIsFind = true;
                                }
                                j++;
                            }
                        }
                    }
                }
                else
                {
                    //go to end of right of array weapon slots
                    if (OldIndex != Seconditeration)
                    {
                        if (WeaponSlots.IsValidIndex(Seconditeration))
                        {
                            if (!WeaponSlots[Seconditeration].NameItem.IsNone())
                            {
                                if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
                                {
                                    //WeaponGood
                                    bIsSuccess = true;
                                    NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
                                    NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
                                    NewCurrentIndex = Seconditeration;
                                }
                                else
                                {
                                    FWeaponInfo myInfo;
                                    UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());

                                    myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

                                    bool bIsFind = false;
                                    int8 j = 0;
                                    while (j < AmmoSlots.Num() && !bIsFind)
                                    {
                                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                                        {
                                            //WeaponGood
                                            bIsSuccess = true;
                                            NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
                                            NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
                                            NewCurrentIndex = Seconditeration;
                                            bIsFind = true;
                                        }
                                        j++;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        //go to same weapon when start
                        if (WeaponSlots.IsValidIndex(Seconditeration))
                        {
                            if (!WeaponSlots[Seconditeration].NameItem.IsNone())
                            {
                                if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
                                {
                                    //WeaponGood, it same weapon do nothing
                                }
                                else
                                {
                                    FWeaponInfo myInfo;
                                    UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());

                                    myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

                                    bool bIsFind = false;
                                    int8 j = 0;
                                    while (j < AmmoSlots.Num() && !bIsFind)
                                    {
                                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
                                        {
                                            if (AmmoSlots[j].Cout > 0)
                                            {
                                                //WeaponGood, it same weapon do nothing
                                            }
                                            else
                                            {
                                                //Not find weapon with amm need init Pistol with infinity ammo
                                                UE_LOG(LogTemp, Error, TEXT("UTDS123InventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
                                            }
                                        }
                                        j++;
                                    }
                                }
                            }
                        }
                    }
                    Seconditeration++;
                }
            }
        }
        else
        {
            int8 iteration = 0;
            int8 Seconditeration = WeaponSlots.Num() - 1;
            while (iteration < WeaponSlots.Num() && !bIsSuccess)
            {
                iteration++;
                int8 tmpIndex = ChangeToIndex - iteration;
                if (WeaponSlots.IsValidIndex(tmpIndex))
                {
                    if (!WeaponSlots[tmpIndex].NameItem.IsNone())
                    {
                        if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
                        {
                            //WeaponGood
                            bIsSuccess = true;
                            NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
                            NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
                            NewCurrentIndex = tmpIndex;
                        }
                        else
                        {
                            FWeaponInfo myInfo;
                            UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());

                            myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

                            bool bIsFind = false;
                            int8 j = 0;
                            while (j < AmmoSlots.Num() && !bIsFind)
                            {
                                if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                                {
                                    //WeaponGood
                                    bIsSuccess = true;
                                    NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
                                    NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
                                    NewCurrentIndex = tmpIndex;
                                    bIsFind = true;
                                }
                                j++;
                            }
                        }
                    }
                }
                else
                {
                    //go to end of LEFT of array weapon slots
                    if (OldIndex != Seconditeration)
                    {
                        if (WeaponSlots.IsValidIndex(Seconditeration))
                        {
                            if (!WeaponSlots[Seconditeration].NameItem.IsNone())
                            {
                                if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
                                {
                                    //WeaponGood
                                    bIsSuccess = true;
                                    NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
                                    NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
                                    NewCurrentIndex = Seconditeration;
                                }
                                else
                                {
                                    FWeaponInfo myInfo;
                                    UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());

                                    myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

                                    bool bIsFind = false;
                                    int8 j = 0;
                                    while (j < AmmoSlots.Num() && !bIsFind)
                                    {
                                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                                        {
                                            //WeaponGood
                                            bIsSuccess = true;
                                            NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
                                            NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
                                            NewCurrentIndex = Seconditeration;
                                            bIsFind = true;
                                        }
                                        j++;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        //go to same weapon when start
                        if (WeaponSlots.IsValidIndex(Seconditeration))
                        {
                            if (!WeaponSlots[Seconditeration].NameItem.IsNone())
                            {
                                if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
                                {
                                    //WeaponGood, it same weapon do nothing
                                }
                                else
                                {
                                    FWeaponInfo myInfo;
                                    UTDS123GameInstance* myGI = Cast<UTDS123GameInstance>(GetWorld()->GetGameInstance());

                                    myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

                                    bool bIsFind = false;
                                    int8 j = 0;
                                    while (j < AmmoSlots.Num() && !bIsFind)
                                    {
                                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
                                        {
                                            if (AmmoSlots[j].Cout > 0)
                                            {
                                                //WeaponGood, it same weapon do nothing
                                            }
                                            else
                                            {
                                                //Not find weapon with amm need init Pistol with infinity ammo
                                                UE_LOG(LogTemp, Error, TEXT("UTDS123InventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
                                            }
                                        }
                                        j++;
                                    }
                                }
                            }
                        }
                    }
                    Seconditeration--;
                }
            }
        }
    }

    if (bIsSuccess)
    {
        SetAdditionalInfoWeapon(OldIndex, OldInfo);
        OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
        //OnWeaponAmmoAviable.Broadcast()
    }


    return bIsSuccess;
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
            if (/*WeaponSlots[i].IndexSlot*/i == IndexWeapon)
            {
                result = WeaponSlots[i].AdditionalInfo;
                bIsFind = true;
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
            result = i/*WeaponSlots[i].IndexSlot*/;
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
            if (/*WeaponSlots[i].IndexSlot*/i == IndexWeapon)
            {
                WeaponSlots[i].AdditionalInfo = NewInfo;
                bIsFind = true;

                OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, NewInfo);
            }
            i++;
        }
        if (!bIsFind)
            UE_LOG(LogTemp, Warning, TEXT("UTDS123InventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("UTDS123InventoryComponent::SetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);
}

