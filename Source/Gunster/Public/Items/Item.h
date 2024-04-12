// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"

#include "Item.generated.h"

UCLASS(Abstract)
class GUNSTER_API AItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AItem();
protected:
	virtual void Interact_Implementation(class APawn* Instigator) override;
public:
};
