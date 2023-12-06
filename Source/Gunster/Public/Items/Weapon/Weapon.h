// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class GUNSTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Mesh",meta = (AllowPrivateAccess="true"))
	class UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MuzzlePosition;

	//FX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* SmokeTrail;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactFlash;
public:
	//Reload Action
	void ReloadMagazine();
	
	//Fire Action
	void StartFire();
	void StopFire();
private:
	//Sounds and VFXs
	void PlayFireSound();
	void PlayFireVFX(FVector& EndPoint);
	void PlayImpactVFX(FHitResult& HitResult);

	//Fire
	void Fire();
	void TrackTrajectory();
	void ResetFire();
	
	//Init Delegate Lambda
	void InitDelegates();
private:
	//for weapon firing
	bool bShouldFire;  //When Player Triggering is True
	bool bCanFire;	//When Gun is Ready to Fire is True
	float FireRate;
	FTimerHandle FireTimerHandle;
	FTimerDelegate ResetFireTimerDelegate;
};
