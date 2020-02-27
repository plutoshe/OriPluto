// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OriPlutoGameMode.h"
#include "OriPlutoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOriPlutoGameMode::AOriPlutoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
