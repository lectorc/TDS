// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDS123GameMode.h"
#include "TDS123PlayerController.h"
#include "TDS123Character.h"
#include "UObject/ConstructorHelpers.h"

ATDS123GameMode::ATDS123GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATDS123PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprints/Characters/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}
