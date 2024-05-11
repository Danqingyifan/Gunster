// Fill out your copyright notice in the Description page of Project Settings.

#include "Action/GunsterAction.h"
#include "Action/GunsterActionComponent.h"


UGunsterAction::UGunsterAction()
	:bIsRunning(false),bAutoStart(false)
{

}

bool UGunsterAction::CanStartAction_Implementation(AActor* InstigatorActor) const
{
	if (bIsRunning)
	{
		return false;
	}

	UGunsterActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
		if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
		{
			return false;
		}
	}
	return true;
}

void UGunsterAction::StartAction_Implementation(AActor* InstigatorActor)
{
	bIsRunning = true;
	UE_LOG(LogTemp, Warning, TEXT("Running : %s"), *GetNameSafe(this));
	UGunsterActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
		Comp->ActiveGameplayTags.AppendTags(GrantTags);
	}
}

void UGunsterAction::StopAction_Implementation(AActor* InstigatorActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Stop : %s"), *GetNameSafe(this));

	ensureAlways(bIsRunning);
	UGunsterActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
		Comp->ActiveGameplayTags.RemoveTags(GrantTags);
	}
	bIsRunning = false;
}

class UWorld* UGunsterAction::GetWorld() const
{
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}

class UGunsterActionComponent* UGunsterAction::GetOwningComponent() const
{
	return Cast<UGunsterActionComponent>(GetOuter());
}
