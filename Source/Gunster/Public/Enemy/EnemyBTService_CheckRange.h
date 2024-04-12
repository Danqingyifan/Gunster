// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "EnemyBTService_CheckRange.generated.h"

/**
 *
 */
UCLASS()
class GUNSTER_API UEnemyBTService_CheckRange : public UBTService
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Key")
	FBlackboardKeySelector LOSKey;


	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
