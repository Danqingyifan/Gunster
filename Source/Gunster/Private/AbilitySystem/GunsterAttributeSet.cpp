// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GunsterAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
UGunsterAttributeSet::UGunsterAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitStamina(100.f);
	InitMaxStamina(100.f);
}

void UGunsterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UGunsterAttributeSet, Health, COND_None, REPNOTIFY_Always); 
	DOREPLIFETIME_CONDITION_NOTIFY(UGunsterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always); 
	DOREPLIFETIME_CONDITION_NOTIFY(UGunsterAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGunsterAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
}

void UGunsterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGunsterAttributeSet, Health, OldHealth);
}

void UGunsterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGunsterAttributeSet, MaxHealth, OldMaxHealth);

}

void UGunsterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGunsterAttributeSet, Stamina, OldStamina);
}

void UGunsterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGunsterAttributeSet, MaxStamina, OldMaxStamina);
}
