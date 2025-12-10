// Fill out your copyright notice in the Description page of Project Settings.


#include "StateEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TDS123/Character/TDS123CharacterHealthComponent.h"

bool UStateEffect::InitObject(AActor* Actor)
{
	myActor = Actor;
	return true;
}

void UStateEffect::DestroyObject()
{
	if (this && this->IsValidLowLevel())
	{
		this->BeginDestroy();
	}
}

bool UTDS123_StateEffect_ExecuteOnce::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);
	return true;
}

void UTDS123_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UTDS123_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if(myActor)
	{ 
		UTDS123_HealthComponent* MyHealthComp = Cast<UTDS123_HealthComponent>(myActor->GetComponentByClass(UTDS123_HealthComponent::StaticClass()));
		if (MyHealthComp)
		{
			MyHealthComp->ChangeHealthValue(Power);
		}
	}

	DestroyObject();
}

bool UTDS123_StateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UTDS123_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTDS123_StateEffect_ExecuteTimer::Execute, Timer, true);
	if (ParticleEffect)
	{
		FName NameBoneToAttached;
		FVector Loc;
		ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetRootComponent(), NameBoneToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}
	return true;
}

void UTDS123_StateEffect_ExecuteTimer::DestroyObject()
{
	ParticleEmitter->DestroyComponent();
	ParticleEmitter = nullptr;

 	Super::DestroyObject();
	
	
}

void UTDS123_StateEffect_ExecuteTimer::Execute()
{
	Super::DestroyObject();

	if (myActor)
	{
		UTDS123_HealthComponent* MyHealthComp = Cast<UTDS123_HealthComponent>(myActor->GetComponentByClass(UTDS123_HealthComponent::StaticClass()));
		if (MyHealthComp)
		{
			MyHealthComp->ChangeHealthValue(Power);
		}
	}

		

}
