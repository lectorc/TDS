// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TDS123_API UStateEffect : public UObject
{
	GENERATED_BODY()

public:

	virtual bool InitObject();
	virtual void DestroyObject();

};
UCLASS()
class TDS123_API UTDS123_StateEffect_ExecuteOnce : public UStateEffect
{
	GENERATED_BODY()

public:
	bool InitObject() override;
	void DestroyObject() override;
};