// Fill out your copyright notice in the Description page of Project Settings.


#include "IGame_Actor.h"

// Add default functionality here for any IIGame_Actor functions that are not pure virtual.

EPhysicalSurface IIGame_Actor::GetSurfaceType()
{
    return EPhysicalSurface();
}






bool IIGame_Actor::AviableForEffectsOnlyCPP()
{
    return true;        
}



TArray<UStateEffect*> IIGame_Actor::GetAllCurrentEffects()
{
	TArray<UStateEffect*> Effect;
	return Effect;
}