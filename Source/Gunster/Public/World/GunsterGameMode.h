// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/DataTable.h"
#include "GunsterGameMode.generated.h"

USTRUCT(BlueprintType)
struct FSpawnBotInfoRow :public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AActor> EnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxSpawnNumber;
};


UCLASS(minimalapi)
class AGunsterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGunsterGameMode();
protected:

	virtual void StartPlay() override;
	UFUNCTION()
	void SpawnBot();
	UFUNCTION()
	void OnQueryFinished(class UEnvQueryInstanceBlueprintWrapper* QueryInstance,EEnvQueryStatus::Type QueryStatus);
	
	UPROPERTY(EditDefaultsOnly,Category = "AI")
	class UDataTable* EnemySpawnDataTable;

private:
	FTimerHandle SpawnBotTimerHandle;
	float SpawnBotInterval;
	UPROPERTY(EditDefaultsOnly)
	class UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditAnywhere)
	class UCurveFloat* SpawnNumberCurve;

	int MaxEnemySpawnNumber;
};

