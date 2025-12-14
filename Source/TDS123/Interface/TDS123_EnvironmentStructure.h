// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDS123/Interface/IGame_Actor.h"
#include "TDS123_EnvironmentStructure.generated.h"

UCLASS()
class TDS123_API ATDS123_EnvironmentStructure : public AActor //public IIGame_Actor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDS123_EnvironmentStructure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	bool AviableForEffects_Implementation();
	//bool AviableForEffectsOnlyCPP() override;
};
