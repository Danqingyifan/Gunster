// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "GunsterAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GUNSTER_API UGunsterAction : public UObject
{
	GENERATED_BODY()
public:
	UGunsterAction();
public:
	UPROPERTY(EditDefaultsOnly,Category = "Tags")
	FGameplayTagContainer GrantTags;
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(EditDefaultsOnly,Category = "Action")
	FName ActionName;

	UFUNCTION(BlueprintNativeEvent,Category = "Action")
	bool CanStartAction(AActor* InstigatorActor) const;

	bool bIsRunning;

	UFUNCTION(BlueprintNativeEvent,Category = "Action")
	void StartAction(AActor* InstigatorActor);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* InstigatorActor);

	virtual class UWorld* GetWorld() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	class UGunsterActionComponent* GetOwningComponent() const;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;
};
 