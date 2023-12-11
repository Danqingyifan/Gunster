// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GunsterCharacter.generated.h"

// Character play FiringAction animation According to this state enum
UENUM(BlueprintType)
enum class EFireState : uint8
{
	EFS_Idle UMETA(DisplayName = "Idle"),
	EFS_Firing UMETA(DisplayName = "Firing"),
	EFS_Reloading UMETA(DisplayName = "Reloading"),
	EFS_Equipping UMETA(DisplayName = "Equipping"),
};

UCLASS(config = Game)
class AGunsterCharacter : public ACharacter
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

//Data Area
private:
	//Weapon Segment
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> DefaultWeaponClass;

	class AWeapon* LeftHoldingWeapon;
	class AWeapon* RightHoldingWeapon;
	
	EFireState FireState;
	bool bIsAiming;
private:
	//Camera Segment
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	float IdleFOV;
	float AimFOV;

//Function Area

//API Part
protected:
	// Basic Segment
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	//Action Segment
	void Dodge();
	void Sprint();
	void Dash();

	//Weapon Segment
	void Aim();
	void StopAim();
	void PullTrigger();
	void ReleaseTrigger();
	void Reload();

	//Camara Segment
	void ZoomCamera(float TargetFOV, float DeltaTime);

//Init Part
private: 
	//Constructor
	void SetUpControllerRotation();
	void SetUpCamera();
	void SetUpCharacterMovement();

	//BeginPlay
	void SpawnDefaultWeapon();

	//Input
	void SetUpInput(class UInputComponent* PlayerInputComponent);
	
//Implementation Part
private:
	class AWeapon* SpawnWeapon(const class USkeletalMeshSocket* Socket, const TSubclassOf<class AWeapon> WeaponClass);
	void AttachWeapon(class AWeapon* Weapon, const class USkeletalMeshSocket* Socket);

public:
	FORCEINLINE EFireState GetFireState() const { return FireState; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
};

