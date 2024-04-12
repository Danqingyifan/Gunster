 // Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBTTaskNode_MeleeAttack.h"
#include "Enemy/Enemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UEnemyBTTaskNode_MeleeAttack::UEnemyBTTaskNode_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UEnemyBTTaskNode_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* EnemyAIController = Cast<AAIController>(OwnerComp.GetAIOwner());

	if (ensure(EnemyAIController))
	{
		AEnemy* EnemyCharacter = Cast<AEnemy>(EnemyAIController->GetPawn());
		if (EnemyCharacter != nullptr)
		{
			AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Target"));
			if (TargetActor == nullptr)
			{
				return EBTNodeResult::Failed;
			}

			FVector FaceDirection = TargetActor->GetActorLocation() - EnemyCharacter->GetActorLocation();
			FRotator FaceRotation = FaceDirection.Rotation();
			EnemyCharacter->SetActorRotation(FaceRotation);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}