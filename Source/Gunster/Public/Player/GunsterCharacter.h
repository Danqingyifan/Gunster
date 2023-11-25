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

private:

	//Camera Segment
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	//Weapon Segment
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AWeapon> HoldingWeaponClass;

protected:
	// Action Segment
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Trigger();
	void Dodge();
	void Sprint();
protected:
	virtual void BeginPlay();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	// Getter Segment
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
private:
	const class USkeletalMeshSocket* RightHandGun;
	const class USkeletalMeshSocket* LeftHandGun;
	class AWeapon* HoldingWeapon;
};

