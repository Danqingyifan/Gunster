// Fill out your copyright notice in the Description page of Project Settings.


#include "Action/GunsterActionEffect.h"
#include "Action/GunsterActionComponent.h"

UGunsterActionEffect::UGunsterActionEffect()
{

}

void UGunsterActionEffect::StartAction_Implementation(AActor* InstigatorActor)
{
	Super::StartAction_Implementation(InstigatorActor);
	if (Duration > 0.0f)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("StopAction"), InstigatorActor);
		GetWorld()->GetTimerManager().SetTimer(DurationHandle, TimerDelegate, Duration, false);
	}
	if (Period > 0.0f)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("ExecutePeriodEffect"), InstigatorActor);
		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, TimerDelegate, Period, true);
	}
}

void UGunsterActionEffect::StopAction_Implementation(AActor* InstigatorActor)
{
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(InstigatorActor);
	}
	Super::StopAction_Implementation(InstigatorActor);

	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);
	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);

	UGunsterActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
		Comp->RemoveAction(this);
	}
}
void UGunsterActionEffect::ExecutePeriodEffect_Implementation(AActor* InstigatorActor)
{

}

