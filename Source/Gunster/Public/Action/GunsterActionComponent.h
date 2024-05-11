// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GunsterActionComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUNSTER_API UGunsterActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGunsterActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY()
	TArray<class UGunsterAction*> ActionArray;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TArray<TSubclassOf<UGunsterAction>> DefaultActionClasses;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor* InstigatorActor, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor* InstigatorActor, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void AddAction(TSubclassOf<class UGunsterAction> ActionClass,AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RemoveAction(class UGunsterAction* Action);

	UPROPERTY(VisibleAnywhere, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;
};
