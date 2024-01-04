// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SMG UMETA(DisplayName = "SMG"),
	EWT_Rifle UMETA(DisplayName = "Rifle")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_OnGround UMETA(DisplayName = "OnGround"),
	EWS_Picking UMETA(DisplayName = "Picking"),
	EWS_InBackpack UMETA(DisplayName = "InBackpack"),
	EWS_Equipped UMETA(DisplayName = "Equipped")
};

UENUM(BlueprintType)
enum class EFireState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Firing UMETA(DisplayName = "Firing"),
	ECS_Reloading UMETA(DisplayName = "Reloading")
};

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MuzzlePosition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	//FX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* SmokeTrail;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactFlash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ReloadSound;
	//HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* CrossHair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WeaponPickUp;

	
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
	void PlayReloadSound();

	//Fire
	void Fire();
	void TrackTrajectory();

private:
	//for weapon firing
	bool bShouldFire;  //When Player Triggering is True
	bool bCanFire;	//When Gun is Ready to Fire is True
	float FireRate;
	float ReloadTime;
	EFireState FireState;
	FTimerHandle TimerHandle;

	//for weapon spawn and character animation
	bool isSingleHanded;

	//Weapon Properties
	EWeaponState WeaponState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	//Ammo
	TMap<EWeaponType, uint8> StartingAmmoMap;
	uint8 StartingAmmo;
	uint8 LeftAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	uint8 ClipCapacity;

public:
	void SetUpWeaponState(EWeaponState State);
	void SetUpFireState(EFireState State);
	uint8 GetLeftAmmo() { return LeftAmmo; }
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
private:
	void SetUpWeaponProperties(EWeaponState State);
};
