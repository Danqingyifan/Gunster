// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GunsterPlayerState.generated.h"

/**
 *
 */
UCLASS()
class GUNSTER_API AGunsterPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UAttributeSet> AttributeSet;

public:
	AGunsterPlayerState();

	FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }
};
