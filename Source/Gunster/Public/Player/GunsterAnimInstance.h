// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GunsterAnimInstance.generated.h"

UCLASS()
class GUNSTER_API UGunsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Owner", meta = (AllowPrivateAccess = "true"))
	class AGunsterCharacter* OwningCharacter;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "State",meta = (AllowPrivateAccess = "true"))
	bool IsAccelerating;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool IsAiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool IsShooting;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateValue", meta = (AllowPrivateAccess = "true"))
	float JogSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateValue", meta = (AllowPrivateAccess = "true"))
	float JogYawOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateValue", meta = (AllowPrivateAccess = "true"))
	FRotator AimOffset;

	void JogAnimUpdate();
	void AimOffsetUpdate();
	void checkIfAiming();

public:
	UFUNCTION(BlueprintPure,Category = "State")
	bool GetIsAiming() const { return IsAiming; }
};
