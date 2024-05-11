// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define ECC_PlayerWeaponChannel ECC_GameTraceChannel2
#define ECC_Player ECC_GameTraceChannel1

static void LogOnScreen(UObject* WorldContext, FString Msg, FColor Color = FColor::Red, float Duration = 5.f)
{
	if (!WorldContext)
	{
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	FString NetPrefix = World->IsNetMode(NM_Client) ? "[Client]" : "[Server]";
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + Msg);
	}
}