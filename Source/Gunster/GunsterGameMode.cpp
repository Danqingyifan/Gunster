// Copyright Epic Games, Inc. All Rights Reserved.

#include "GunsterGameMode.h"
#include "GunsterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGunsterGameMode::AGunsterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
