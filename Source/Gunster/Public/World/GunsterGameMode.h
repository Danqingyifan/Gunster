// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GunsterGameMode.generated.h"

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
private:
	FTimerHandle SpawnBotTimerHandle;
	float SpawnBotInterval;
	UPROPERTY(EditDefaultsOnly)
	class UEnvQuery* SpawnBotQuery;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> EnemyClass;
	UPROPERTY(EditAnywhere)
	class UCurveFloat* SpawnNumberCurve;

	int MaxEnemySpawnNumber;

};

