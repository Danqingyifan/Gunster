// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GunsterPlayerState.h"

#include "AbilitySystem/GunsterAttributeSet.h"
#include "AbilitySystem/GunsterAbilitySystemComponent.h"


AGunsterPlayerState::AGunsterPlayerState()
{
	NetUpdateFrequency = 100;

	AbilitySystemComponent = CreateDefaultSubobject<UGunsterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	check(AbilitySystemComponent);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGunsterAttributeSet>(TEXT("AttributeSet"));
}
