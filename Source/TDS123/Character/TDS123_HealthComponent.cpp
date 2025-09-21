// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS123_HealthComponent.h"

// Sets default values for this component's properties
UTDS123_HealthComponent::UTDS123_HealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTDS123_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTDS123_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UTDS123_HealthComponent::GetCurrentHealth()
{
	return Health;
}

void UTDS123_HealthComponent::ChangeHealthValue(float ChangeValue)
{
	ChangeValue = ChangeValue * CoefDamage;

	Health += ChangeValue;
	if (Health > 100.0f)
	{
		Health = 100.0f;
	}
	else
	{
		if (Health < 0.0f)
		{
			OnDead.Broadcast();
		}
	}

	OnHealthChange.Broadcast(Health, ChangeValue);
}

void UTDS123_HealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
}


