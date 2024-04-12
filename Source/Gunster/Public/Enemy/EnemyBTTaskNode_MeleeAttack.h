// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyBTTaskNode_MeleeAttack.generated.h"

/**
 *
 */
UCLASS()
class GUNSTER_API UEnemyBTTaskNode_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UEnemyBTTaskNode_MeleeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
