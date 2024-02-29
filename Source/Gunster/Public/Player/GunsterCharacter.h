// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnBulletHitInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GunsterCharacter.generated.h"

// Character play FiringAction animation According to this state enum
UCLASS(config = Game)
class AGunsterCharacter : public ACharacter, public IOnBulletHitInterface
{
	GENERATED_BODY()

public:
	AGunsterCharacter();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	// Getter Segment
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
private:
	//Weapon Segment
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquippedWeapon;

	//Montage Segment
	class UAnimInstance* AnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* StrafeMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

private:
	//Camera Segment
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	float IdleFOV;
	float AimFOV;

private:
	//FireState
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireState", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireState", meta = (AllowPrivateAccess = "true"))
	bool bIsShooting;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireState", meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;

	//Health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	class USoundCue* MeleeImpactSound;
public:
	// Basic Segment
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	//Action Segment
	void AimLocomotion(bool isAiming);
	void Dodge();
	void Sprint();
	void Dash();
	UFUNCTION(BlueprintNativeEvent)
	void SwitchWeapon();
	void FinishSwitchWeapon();
	//Weapon Segment
	void Aim();
	void StopAim();
	void PullTrigger();
	void ReleaseTrigger();
	void Reload();

	//Camara Segment
	void ZoomIfAim(bool isAiming, float DeltaTime);
	void ZoomCamera(float TargetFOV, float DeltaTime);

	void Die();
private:
	//Constructor
	void SetUpControllerRotation();
	void SetUpCamera();
	void SetUpCharacterMovement();

	//BeginPlay
	void SpawnDefaultWeapon();

	//Input
	void SetUpInput(class UInputComponent* PlayerInputComponent);
private:
	class AWeapon* SpawnWeapon(const class USkeletalMeshSocket* Socket, const TSubclassOf<class AWeapon> WeaponClass);
	void AttachWeapon(class AWeapon* Weapon, const class USkeletalMeshSocket* Socket);

public:
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE bool GetIsShooting() const { return bIsShooting; }
	FORCEINLINE USoundCue* GetMeleeImpactSound() const { return MeleeImpactSound; }
	void OnBulletHit_Implementation(FHitResult HitResult) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};

