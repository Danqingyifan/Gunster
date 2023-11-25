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
	if (OwningCharacter == nullptr)
	{
		OwningCharacter = Cast<AGunsterCharacter>(TryGetPawnOwner());
	}
	if (OwningCharacter)
	{
		FVector Velocity = OwningCharacter->GetVelocity();
		Velocity.Z = 0;
		JogSpeed = Velocity.Size();
		if (OwningCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() != 0)
		{
			IsAccelerating = true;
		}
		else
		{
			IsAccelerating = false;
		}

		//YawOffset Setup
		FRotator AimRotation = OwningCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(OwningCharacter->GetVelocity());
		
		YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		FString YawOffsetMessage = FString::Printf(TEXT("YawOffsetMessage: %f"), MovementRotation.Yaw);
		UE_LOG(LogTemp, Display, TEXT("YawOffsetMessage: %f"), YawOffset);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::White, YawOffsetMessage);
		}
	}
}