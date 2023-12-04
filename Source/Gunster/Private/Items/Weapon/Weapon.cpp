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

AWeapon::AWeapon()
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

void AWeapon::Fire()
{
	PlayFireSound();
	PlayFireVFX();
	TraceLine();

}

void AWeapon::TraceLine()
{
	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;
	if (AGunsterCharacter* PlayerCharacter = Cast<AGunsterCharacter>(GetAttachParentActor()))
	{
		if (AGunsterPlayerController* PlayerController = Cast<AGunsterPlayerController>(PlayerCharacter->GetController()))
		{
			PlayerController->DeprojectCrossHairToWorld(CrossHairWorldPosition,CrossHairWorldDirection);
		}
	}

	FHitResult CrossHairTrace;
	FVector CrossHairStart{ CrossHairWorldPosition };
	FVector CrossHairEnd{ CrossHairWorldPosition + CrossHairWorldDirection * 1'000 };
	GetWorld()->LineTraceSingleByChannel(CrossHairTrace, CrossHairStart, CrossHairEnd, ECollisionChannel::ECC_Visibility);

	FHitResult BarrelTrace;
	FVector BarrelStart{ MuzzlePosition->GetComponentTransform().GetLocation() };
	FVector BarrelEnd{ CrossHairEnd };
	GetWorld()->LineTraceSingleByChannel(BarrelTrace, BarrelStart, BarrelEnd, ECollisionChannel::ECC_Visibility);

	FHitResult FireHit { BarrelTrace };
	DrawDebugLine(GetWorld(), BarrelStart, FireHit.ImpactPoint, FColor::Red, false, 3.f);
	DrawDebugSphere(GetWorld(), FireHit.ImpactPoint, 15.f, 24, FColor::Green, false, 3.f);

	if (FireHit.bBlockingHit)
	{
		PlayImpactVFX(FireHit);
	}

}

void AWeapon::PlayFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AWeapon::PlayFireVFX()
{
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzlePosition->GetComponentTransform());
	}
	if (SmokeTrail)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeTrail, MuzzlePosition->GetComponentTransform());
	}
}

void AWeapon::PlayImpactVFX(FHitResult& HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFlash, HitResult.Location);
}
