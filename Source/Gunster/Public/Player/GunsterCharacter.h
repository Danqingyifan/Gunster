// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interfaces/OnBulletHitInterface.h"

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	class AGunsterPlayerController* GunsterPlayerController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	class UInteractableComponent* InteractableComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	class UCharacterAttributesComponent* CharacterAttributesComponent;

	//Montage Segment
	class UAnimInstance* AnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* StrafeMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

private:
	//Camera Segment
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	float IdleFOV;
	float AimFOV;

public:
	//FireState
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireState", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireState", meta = (AllowPrivateAccess = "true"))
	bool bIsShooting;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireState", meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;

	//Health
	// TODO : Move the Attributes to One Character Attribute Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	class USoundCue* MeleeImpactSound;
public:
	// Basic Segment
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	//Action Segment
	void Interact();
	void AimLocomotion(bool isAiming);
	void Dodge();
	void Sprint();
	void Dash();
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
	UFUNCTION(BlueprintCallable)
	void DeathFinish();
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
	FORCEINLINE class USoundCue* GetMeleeImpactSound() const { return MeleeImpactSound; }
	void OnBulletHit_Implementation(const FHitResult& HitResult) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	UFUNCTION(Server, Unreliable, WithValidation,BlueprintCallable)
	void ServerRPCSpawnBox(int arg);
	UPROPERTY(EditAnywhere)
	class UStaticMesh* BoxMesh;
};

