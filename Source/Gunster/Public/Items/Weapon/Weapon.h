// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "OnBulletHitInterface.h"
#include "Weapon.generated.h"

//FireState
UENUM(BlueprintType)
enum class EFireState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Firing UMETA(DisplayName = "Firing"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

//WeaponType
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SMG UMETA(DisplayName = "SMG"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FWeaponDataTable :public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;
	//Combat 
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	uint8 ClipCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 StartingAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadShotMultiplier;

	//Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* MuzzlePosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionBox;

	//FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* ReloadSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* SmokeTrail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ImpactFlash;

	//HUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* CrossHair;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* WeaponIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* AmmoTypeIcon;
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_OnGround UMETA(DisplayName = "OnGround"),
	EWS_Picking UMETA(DisplayName = "Picking"),
	EWS_InBackpack UMETA(DisplayName = "InBackpack"),
	EWS_Equipped UMETA(DisplayName = "Equipped")
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
	class USoundCue* ReloadSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* SwitchSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* OutOfAmmoSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* SmokeTrail;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactFlash;

	//HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* CrossHair;

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
	void PlayDefaultImpactVFX(FHitResult& HitResult);
	void PlayReloadSound();
	void PlayOutOfAmmoSound();

	//Fire
	void Fire();
	FHitResult TrackTrajectory();

private:
	class AGunsterCharacter* WeaponOwner;

	//for weapon firing
	bool bShouldFire;  //When Player Triggering is True
	bool bCanFire;	//When Gun is Ready to Fire is True
	EFireState FireState;
	FTimerHandle TimerHandle;

	//DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	class UDataTable* WeaponDataTable;
	
	//Weapon Properties
	EWeaponState WeaponState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FString WeaponName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float ReloadTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UTexture2D* WeaponIcon;

	//Ammo
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category ="Ammo", meta = (AllowPrivateAccess = "true"))
	int32 StartingAmmo; //The total amount of ammo the player has at the beginning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 WeaponAmmo;	//The amount of ammo left in the clip
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 ClipCapacity; //The amount of ammo the clip can hold
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UTexture2D* AmmoTypeIcon;

	//Damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HeadShotMultiplier;

public:
	void SetUpWeaponState(EWeaponState State);
	uint8 GetLeftAmmo() { return WeaponAmmo; }

	UFUNCTION()
	void OnConstruction(const FTransform& Transform) override;

	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE EFireState GetFireState() const { return FireState; }
	FORCEINLINE void SetFireSound(USoundCue* Sound) { FireSound = Sound; }
	FORCEINLINE void SetReloadSound(USoundCue* Sound) { ReloadSound = Sound; }
	FORCEINLINE void SetSmokeTrail(UParticleSystem* Particle) { SmokeTrail = Particle; }
	FORCEINLINE void SetWeaponMesh(UStaticMesh* Mesh) { WeaponMesh->SetStaticMesh(Mesh); }
	FORCEINLINE bool GetCanReload() const { return StartingAmmo > 0 && WeaponAmmo < ClipCapacity && FireState != EFireState::ECS_Reloading; }
	FORCEINLINE bool GetCanFire() const { return bCanFire; }
private:
	void SetUpWeaponProperties(EWeaponState State);
};
