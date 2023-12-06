// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/GunsterAnimInstance.h"
#include "Player/GunsterCharacter.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
void UGunsterAnimInstance::NativeInitializeAnimation()
{
	if (OwningCharacter == nullptr)
	{
		OwningCharacter = Cast<AGunsterCharacter>(TryGetPawnOwner());
	}
}

void UGunsterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{	
	AimOffsetAnimUpdate();
	JogAnimUpdate();
}

void UGunsterAnimInstance::JogAnimUpdate()
{
	if (OwningCharacter == nullptr)
	{
		OwningCharacter = Cast<AGunsterCharacter>(TryGetPawnOwner());
	}
	if (OwningCharacter)
	{	
		//JogSpeed Setup
		FVector Velocity = OwningCharacter->GetVelocity();
		Velocity.Z = 0;
		JogSpeed = Velocity.Size();
		// if Accelerating
		if (OwningCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() != 0)
		{
			IsAccelerating = true;
		}
		else
		{
			IsAccelerating = false;
		}
		//YawOffset Setup
		FRotator MovementRotator = UKismetMathLibrary::MakeRotFromX(OwningCharacter->GetVelocity());
		FRotator ToOrientationRotator = OwningCharacter->GetActorForwardVector().ToOrientationRotator();
		JogYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotator, ToOrientationRotator).Yaw;
	}
}

void UGunsterAnimInstance::AimOffsetAnimUpdate()
{
	if (OwningCharacter == nullptr)
	{
		OwningCharacter = Cast<AGunsterCharacter>(TryGetPawnOwner());
	}
	if (OwningCharacter)
	{	
		if (AController* Controller = OwningCharacter->GetController())
		{	
			FRotator ControlRotator = Controller->GetControlRotation();
			FRotator ToOrientationRotator = OwningCharacter->GetActorForwardVector().ToOrientationRotator();
			AimRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotator, ToOrientationRotator);
		}
	}
}

