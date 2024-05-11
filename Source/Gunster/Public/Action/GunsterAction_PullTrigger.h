// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action/GunsterAction.h"
#include "GunsterAction_PullTrigger.generated.h"

/**
 * 
 */
UCLASS()
class GUNSTER_API UGunsterAction_PullTrigger : public UGunsterAction
{
	GENERATED_BODY()
	
public:
	virtual void StartAction_Implementation(AActor* InstigatorActor) override;
};
