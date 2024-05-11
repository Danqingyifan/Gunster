// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action/GunsterAction.h"
#include "GunsterActionEffect.generated.h"

/**
 *
 */
UCLASS()
class GUNSTER_API UGunsterActionEffect : public UGunsterAction
{
	GENERATED_BODY()

public:
	UGunsterActionEffect();

	void StartAction_Implementation(AActor* InstigatorActor) override;

	void StopAction_Implementation(AActor* InstigatorActor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float Period;

	FTimerHandle DurationHandle;
	FTimerHandle PeriodHandle;

	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	void ExecutePeriodEffect(AActor* InstigatorActor);
};
