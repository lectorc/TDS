// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS123/Character/TDS123CharacterHealthComponent.h"
#include "TDS123CharacterHealthComponent.h"

void UTDS123CharacterHealthComponent::ChangeHealthValue(float ChangeValue)
{
	
	
	float CurrentDamage = ChangeValue * CoefDamage;

	if (Shield > 0.0f && (ChangeValue < 0.0f))
	{
		ChangeShieldValue(ChangeValue);
		
		if (Shield < 0.0f)
		{
			 //FX
			UE_LOG(LogTemp,Warning, TEXT("ATDS123CharacterHealthComponent::ChangeHealthValue - Shield < 0"))
		}
	}
	else
	{
		Super::ChangeHealthValue(ChangeValue);
	}
}

float UTDS123CharacterHealthComponent::GetCurrentShield()
{
	return Shield;
}

void UTDS123CharacterHealthComponent::ChangeShieldValue(float ChangeValue)
{
	
	Shield += ChangeValue;

	

	if (Shield > 100.0f)
	{
		Shield = 100.0f;
	}
	else
	{
		if (Shield < 0.0f)
			Shield = 0.0f;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CollDownShieldTimer, this, &UTDS123CharacterHealthComponent::CoolDownShieldEnd, CoolDownShieldRecoveryTime, false);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
	}

	OnShieldChange.Broadcast(Shield, ChangeValue);
}

void UTDS123CharacterHealthComponent::CoolDownShieldEnd()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRecoveryRateTimer, this, &UTDS123CharacterHealthComponent::RecoveryShield, ShieldRecoverRate, true);
	}


}

void UTDS123CharacterHealthComponent::RecoveryShield()
{
	float tmp = Shield;
	tmp = tmp + ShieldRecoverValue;
	if (tmp > 100.0f)
	{
		Shield = 100.0f;
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
		}
	}
	else
	{
		Shield = tmp;
	}

	OnShieldChange.Broadcast(Shield, ShieldRecoverValue);
}
