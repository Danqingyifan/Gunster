// Copyright Epic Games, Inc. All Rights Reserved.

#include "World/GunsterGameMode.h"
#include "Player/GunsterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Enemy/Enemy.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "Engine/DataTable.h"

AGunsterGameMode::AGunsterGameMode()
{
	SpawnBotInterval = 5.f;
}

void AGunsterGameMode::StartPlay()
{
	Super::StartPlay();
	GetWorldTimerManager().SetTimer(SpawnBotTimerHandle, this, &AGunsterGameMode::SpawnBot, SpawnBotInterval, true);
}

void AGunsterGameMode::SpawnBot()
{
	//check if we have reached the maximum number of enemies
	int CurrentEnemyNumber = 0;
	for (TActorIterator<AEnemy> It(GetWorld()); It; ++It)
	{
		AEnemy* Bot = *It;
		if (Bot)
		{
			CurrentEnemyNumber++;
		}
	}

	if (SpawnNumberCurve)
	{
		MaxEnemySpawnNumber = SpawnNumberCurve->GetFloatValue(GetWorld()->GetTimeSeconds());
	}

	if (CurrentEnemyNumber >= MaxEnemySpawnNumber)
	{
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AGunsterGameMode::OnQueryFinished);
	}
}

void AGunsterGameMode::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Query Failed"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsValidIndex(0))
	{
		FVector Location = QueryInstance->GetResultsAsLocations()[0];
		if (EnemySpawnDataTable)
		{
			TArray<FSpawnBotInfoRow*> Rows;
			EnemySpawnDataTable->GetAllRows("", Rows);

			int32 RandomIndex = FMath::RandRange(0,Rows.Num() - 1);
			FSpawnBotInfoRow* SelectedRow = Rows[RandomIndex];

			GetWorld()->SpawnActor<AActor>(SelectedRow->EnemyClass, Location, FRotator::ZeroRotator);
		}
	}
}
