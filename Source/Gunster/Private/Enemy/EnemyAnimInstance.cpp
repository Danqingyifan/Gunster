// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/Enemy.h"

#include "GameFramework/CharacterMovementComponent.h"

UEnemyAnimInstance::UEnemyAnimInstance()
	:bIsInAir(false), bIsAccelerating(false), Speed(0.f)
{

}

void UEnemyAnimInstance::NativeInitializeAnimation()
{

}

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}
	else
	{
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();
		bIsAccelerating = Enemy->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
	}
}
