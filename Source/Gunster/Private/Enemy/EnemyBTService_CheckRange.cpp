// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBTService_CheckRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void UEnemyBTService_CheckRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComponent))
	{
		// Get the player character
		AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject("Target"));
		if (Target)
		{
			AAIController* AIController = OwnerComp.GetAIOwner();
			if (AIController)
			{
				APawn* AIPawn = AIController->GetPawn();
				if (ensure(AIPawn)) 
				  {
					// Get the distance between the player and the enemy
					float Distance = FVector::Dist(Target->GetActorLocation(), AIPawn->GetActorLocation());
					// Set the distance to the blackboard
					bool bIsInRange = Distance < 300.0f;
					bool bCanSeeTarget = AIController->LineOfSightTo(Target);
					BlackboardComponent->SetValueAsBool(LOSKey.SelectedKeyName, bIsInRange && bCanSeeTarget);
				}
			}
		}
	}
}