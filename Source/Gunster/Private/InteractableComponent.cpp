// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComponent.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/InteractableInterface.h"


// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	InteractRange = 200.f;
	// ...
}


// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractableComponent::InteractWith()
{
	ServerInteractWith();
}

void UInteractableComponent::ServerInteractWith_Implementation()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	TArray<FHitResult> Hits;

	FVector EyeLocation;
	FRotator EyeRotation;
	GetOwner()->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + EyeRotation.Vector() * InteractRange;

	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(30.f);

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, CollisionShape);

	FColor TraceColor = bBlockingHit ? FColor::Green : FColor::Red;
	for (FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			APawn* Interactor = Cast<APawn>(GetOwner());
			if (HitActor->Implements<UInteractableInterface>())
			{
				IInteractableInterface::Execute_Interact(HitActor, Interactor);
				// Debug
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 30.f, 32, TraceColor, false, 2.f);
				FString HitLocation = FString::Printf(TEXT("Hit at %s"), *Hit.ImpactPoint.ToString());
				DrawDebugString(GetWorld(), Hit.ImpactPoint, HitLocation, nullptr, TraceColor, 2.f);

				break;
			}
		}
	}
	DrawDebugLine(GetWorld(), EyeLocation, End, TraceColor, false, 2.f, 0, 1.f);
}

