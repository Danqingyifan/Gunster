// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"

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
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	if (MuzzleFlash && SmokeTrail)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzlePosition->GetComponentTransform());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeTrail, MuzzlePosition->GetComponentTransform());
	}

	FHitResult FireHit;
	FVector Start{ MuzzlePosition->GetComponentTransform().GetLocation() };
	FVector End{ Start + MuzzlePosition->GetComponentTransform().GetRotation().GetForwardVector() * 50'000.f};
	GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);
	
	if (FireHit.bBlockingHit)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFlash, FireHit.GetComponent()->GetComponentLocation());
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f);
		DrawDebugSphere(GetWorld(), End, 15.f, 24, FColor::Green, false, 3.f);
	}
}