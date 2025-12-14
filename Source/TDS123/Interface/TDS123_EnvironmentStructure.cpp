// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS123/Interface/TDS123_EnvironmentStructure.h"
#include "TDS123_EnvironmentStructure.h"

// Sets default values
ATDS123_EnvironmentStructure::ATDS123_EnvironmentStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATDS123_EnvironmentStructure::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDS123_EnvironmentStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATDS123_EnvironmentStructure::AviableForEffects_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ATDS123EnvironmentStructure::AviableForEffects_Implementation"));
	return true;
}

//bool ATDS123_EnvironmentStructure::AviableForEffectsOnlyCPP()
//{
//	UE_LOG(LogTemp, Warning, TEXT("ATDS123EnvironmentStructure::AviableForEffectsOnlyCPP"))
//	return true;
//}

