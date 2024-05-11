// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GunsterAttributeSet.generated.h"

#define ATRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class GUNSTER_API UGunsterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGunsterAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = "OnRep_Health",Category="Vital Attributes")
	FGameplayAttributeData Health;
	ATRIBUTE_ACCESSORS(UGunsterAttributeSet, Health);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_MaxHealth", Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATRIBUTE_ACCESSORS(UGunsterAttributeSet, MaxHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_Stamina", Category = "Vital Attributes")
	FGameplayAttributeData Stamina;
	ATRIBUTE_ACCESSORS(UGunsterAttributeSet, Stamina);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_MaxStamina", Category = "Vital Attributes")
	FGameplayAttributeData MaxStamina;
	ATRIBUTE_ACCESSORS(UGunsterAttributeSet, MaxStamina);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const;
};

//RepNotify has to be a UFUNCTION
//Can only take zero and one arguments(if one,it need to be same as variable's class)
//if one, it will be called with the old value of the variable
