// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GunsterCharacter.generated.h"


UCLASS(config = Game)
class AGunsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGunsterCharacter();

protected:
	virtual void BeginPlay();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	// Getter Segment
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
private:
	//Weapon Segment
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapoon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> DefaultWeaponClass;

	const class USkeletalMeshSocket* RightHandSocket;
	const class USkeletalMeshSocket* LeftHandSocket;

	class AWeapon* LeftHoldingWeapon;
	class AWeapon* RightHoldingWeapon;
	
	bool isSingleHolding;
	bool isAiming;

private:
	//Camera Segment
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


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
	void PullTrigger();
	void ReleaseTrigger();
	void Reload();
//Init Part
private: 
	void InitWeapon();
	void SetUpControllerRotation();
	void SetUpCamera();
	void SetUpCharacterMovement();


//Implementation Part
private:
	class AWeapon* SpawnWeapon(const class USkeletalMeshSocket* Socket, const TSubclassOf<class AWeapon> WeaponClass);
	void AttachWeapon(class AWeapon* Weapon, const class USkeletalMeshSocket* Socket);

};

