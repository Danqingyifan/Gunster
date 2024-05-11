// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GUNSTER_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	FORCEINLINE FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Weapon", GetFName());
	}
};
