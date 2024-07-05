#include "Action/GunsterActionComponent.h"
#include "Action/GunsterAction.h"

UGunsterActionComponent::UGunsterActionComponent()
{

}


// Called when the game starts
void UGunsterActionComponent::BeginPlay()
{
	Super::BeginPlay();
	for (TSubclassOf<UGunsterAction> ActionClass : DefaultActionClasses)
	{
		AddAction(ActionClass,GetOwner());
	}
}


void UGunsterActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UGunsterActionComponent::StartActionByName(AActor* InstigatorActor, FName ActionName)
{
	for (UGunsterAction* Action : ActionArray)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStartAction(InstigatorActor))
			{
				FString FailMsg = FString::Printf(TEXT("Can't start action %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FailMsg);
				continue;
			}
			Action->StartAction(InstigatorActor);
			return true;
		}
	}
	return false;
}

bool UGunsterActionComponent::StopActionByName(AActor* InstigatorActor, FName ActionName)
{
	for (UGunsterAction* Action : ActionArray)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->bIsRunning)
			{
				Action->StopAction(InstigatorActor);
				return true;
			}
		}
	}
	return false;
}

void UGunsterActionComponent::AddAction(TSubclassOf<class UGunsterAction> ActionClass,AActor* InstigatorActor)
{
	if (!ensure(ActionClass))
	{
		return;
	}
	UGunsterAction* NewAction = NewObject<UGunsterAction>(this, ActionClass);
	if (ensure(NewAction))
	{
		ActionArray.Add(NewAction);
		if (NewAction->bAutoStart && NewAction->CanStartAction(InstigatorActor))
		{
			NewAction->StartAction(InstigatorActor);
		}
	}
}

void UGunsterActionComponent::RemoveAction(class UGunsterAction* Action)
{
	if (!ensure(Action && !Action->bIsRunning))
	{
		return;
	}
	ActionArray.Remove(Action);
}

