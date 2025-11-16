// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TDS123/StateEffect.h"
#include "StateEffect.generated.h"

/**
 * 
 */
UCLASS()
class TDS123_API UStateEffect : public UObject
{
	GENERATED_BODY()
public:
	virtual bool InitObject(APawn* Pawn);
	virtual void ExecuteEffect(float DeltaTime);
	virtual void DestroyObject();

};
