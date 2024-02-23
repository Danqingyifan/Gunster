// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnBulletHitInterface.h"
#include "GameFramework/Actor.h"
#include "Explosive.generated.h"

UCLASS()
class GUNSTER_API AExplosive : public AActor , public IOnBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosive();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnBulletHit_Implementation(FHitResult HitResult) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ImpactEffects", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ExplosionSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ImpactEffects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ExplosionParticles;
};
