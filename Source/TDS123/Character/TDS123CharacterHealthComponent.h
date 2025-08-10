// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDS123/Character/TDS123_HealthComponent.h"
#include "TDS123CharacterHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class TDS123_API UTDS123CharacterHealthComponent : public UTDS123_HealthComponent
{
	GENERATED_BODY()
public:
    void ChangeCurrentHealth(float ChangeValue) override;
};
