// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"
#include "Player/GunsterCharacter.h"
#include "Player/GunsterPlayerController.h"

#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"

AWeapon::AWeapon()
	:bCanFire(true),bShouldFire(false),FireRate(0.1f) // Fire Params Init
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	MuzzlePosition = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePosition"));
	SetRootComponent(WeaponMesh);
	MuzzlePosition->SetupAttachment(RootComponent);
	
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeapon::StartFire()
{
	bShouldFire = true;
	bCanFire = true;
	Fire();
}

void AWeapon::StopFire()
{
	bShouldFire = false;
}

void AWeapon::TrackTrajectory()
{
	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;
	if (AGunsterCharacter* PlayerCharacter = Cast<AGunsterCharacter>(GetAttachParentActor()))
	{
		if (AGunsterPlayerController* PlayerController = Cast<AGunsterPlayerController>(PlayerCharacter->GetController()))
		{
			PlayerController->DeprojectCrossHairToWorld(CrossHairWorldPosition, CrossHairWorldDirection);
		}
	}

	FHitResult CrossHairTrace;
	FVector CrossHairStart{ CrossHairWorldPosition };
	FVector CrossHairEnd{ CrossHairWorldPosition + CrossHairWorldDirection * 1'000 };
	GetWorld()->LineTraceSingleByChannel(CrossHairTrace, CrossHairStart, CrossHairEnd, ECollisionChannel::ECC_Visibility);
	if (CrossHairTrace.bBlockingHit)
	{
		CrossHairEnd = CrossHairTrace.ImpactPoint;
	}

	FHitResult BarrelTrace;
	FVector BarrelStart{ MuzzlePosition->GetComponentTransform().GetLocation() };
	FVector BarrelEnd{ CrossHairEnd };
	GetWorld()->LineTraceSingleByChannel(BarrelTrace, BarrelStart, BarrelEnd, ECollisionChannel::ECC_Visibility);

	FHitResult FireHit{ BarrelTrace };

	// Debug and Effect
	PlayFireSound();
	PlayFireVFX(BarrelEnd);
	DrawDebugLine(GetWorld(), BarrelStart, BarrelEnd, FColor::Red, false, 3.f);
	if (FireHit.bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), FireHit.ImpactPoint, 15.f, 24, FColor::Green, false, 3.f);
		PlayImpactVFX(FireHit);
	}

}
void AWeapon::Fire()
{
	if (bCanFire)
	{
		TrackTrajectory();
		bCanFire = false;

		GetWorld()->GetTimerManager().SetTimer
		(
			FireTimerHandle,
			this,
			&AWeapon::ResetFire,
			FireRate
		);
	}
}
void AWeapon::ResetFire()
{	
	if (bShouldFire)
	{
		bCanFire = true;
	}
}

/*
void AWeapon::StartFireTimer()
{
	FTimerDelegate FireDelegate;
	FireDelegate.BindLambda([this]() {
		bShouldFire = true;
	});
}
*/

void AWeapon::PlayFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AWeapon::PlayFireVFX(FVector& EndPoint)
{
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzlePosition->GetComponentTransform());
	}
	if (SmokeTrail)
	{
		UParticleSystemComponent* SmokeTrailComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeTrail, MuzzlePosition->GetComponentTransform());
		if (SmokeTrailComponent)
		{
			SmokeTrailComponent->SetVectorParameter("Target", EndPoint);
		}
	}
}

void AWeapon::PlayImpactVFX(FHitResult& HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFlash, HitResult.Location);
}
