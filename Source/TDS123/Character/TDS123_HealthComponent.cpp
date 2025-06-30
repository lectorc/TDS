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

void UTDS123_HealthComponent::ReceiveDamage(float Damage)
{
	Health -= Damage;
	if (Health < 0.0f)
	{
		DeadEvent();
	}
	//OnHealthChange.Broadcast(Health);
}

void UTDS123_HealthComponent::DeadEvent_Implementation()
{
}

