// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesComponent.h"

// Sets default values for this component's properties
UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	// ...
}


// Called every frame
void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCharacterAttributesComponent::ApplyHealthChange(float DeltaHealth)
{
	Health += DeltaHealth;
	//Clamp health to [0,MaxHealth]
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);

	OnHealthChange.Broadcast(GetOwner(), this, Health, DeltaHealth);

	return true;
}


UCharacterAttributesComponent* UCharacterAttributesComponent::GetAttributesComponent(AActor* TargetActor)
{
	if (TargetActor)
	{
		return Cast<UCharacterAttributesComponent>(TargetActor->GetComponentByClass(UCharacterAttributesComponent::StaticClass()));
	}
	return nullptr;
}
