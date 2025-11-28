// Fill out your copyright notice in the Description page of Project Settings.


#include "StateEffect.h"

bool UStateEffect::InitObject()
{
	return true;
}

void UStateEffect::DestroyObject()
{
}

bool UTDS123_StateEffect_ExecuteOnce::InitObject()
{
	Super::InitObject();
}

void UTDS123_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}
