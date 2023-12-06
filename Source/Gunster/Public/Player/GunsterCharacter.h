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
	// Action Segment
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PullTrigger();
	void ReleaseTrigger();
	void Dodge();
	void Sprint();
	void Reload();
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
	class AWeapon* HoldingWeapon;

private:
	//Camera Segment
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
};

