// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"
#include "Player/GunsterCharacter.h"
#include "Player/GunsterPlayerController.h"

#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"

AWeapon::AWeapon()
	:bShouldFire(false), bCanFire(true), FireRate(0.1f), ReloadTime(1.5f) // Fire Params Init
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	MuzzlePosition = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePosition"));
	MuzzlePosition->SetupAttachment(RootComponent);

	//For detect if Character should LineTrace the gun to pickup
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	LeftAmmo = MagazineCapacity;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//API
void AWeapon::StartFire()
{
	bShouldFire = true;
	Fire();
}

void AWeapon::StopFire()
{
	bShouldFire = false;
}

void AWeapon::ReloadMagazine()
{
	LeftAmmo = MagazineCapacity;
	PlayReloadSound();
}

void AWeapon::SetUpWeaponState(EWeaponState State)
{
	switch (State)
	{
	case EWeaponState::EWS_OnGround:
		SetUpWeaponProperties(EWeaponState::EWS_Picking);
		break;
	case EWeaponState::EWS_Equipped:
		SetUpWeaponProperties(EWeaponState::EWS_Equipped);
		break;
	case EWeaponState::EWS_InBackpack:
		SetUpWeaponProperties(EWeaponState::EWS_InBackpack);
		break;
	default:
		SetUpWeaponProperties(EWeaponState::EWS_OnGround);
		break;
	}
	WeaponState = State;
}


// Implementation

//Weapon Fire
void AWeapon::Fire()
{
	if (bCanFire)
	{
		bCanFire = false;
		if (LeftAmmo > 0)
		{
			TrackTrajectory();
			LeftAmmo--;
			//Using Lambda Delegate
			GetWorld()->GetTimerManager().SetTimer
			(
				TimerHandle,
				([this]() {
					bCanFire = true;
					}),
				FireRate,
				false
			);
			UE_LOG(LogTemp, Warning, TEXT("AMMO: %d"), LeftAmmo);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer
			(
				TimerHandle,
				([this]() {
					ReloadMagazine();
					bCanFire = true;
					}),
				ReloadTime,
				false
			);
		}
	}
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
	FVector CrossHairEnd{ CrossHairStart + CrossHairWorldDirection * 1'000 };
	GetWorld()->LineTraceSingleByChannel(CrossHairTrace, CrossHairStart, CrossHairEnd, ECollisionChannel::ECC_Visibility);
	if (CrossHairTrace.bBlockingHit)
	{
		CrossHairEnd = CrossHairTrace.ImpactPoint;
	}

	FHitResult BarrelTrace;
	FVector BarrelStart{ MuzzlePosition->GetComponentTransform().GetLocation() };
	FVector BarrelEnd{ CrossHairEnd + (CrossHairTrace.ImpactPoint - BarrelStart).GetSafeNormal() * 10.f };
	// Need one extra forward vector to make sure the bullet will hit the target

	GetWorld()->LineTraceSingleByChannel(BarrelTrace, BarrelStart, BarrelEnd, ECollisionChannel::ECC_Visibility);

	FHitResult FireHit{ BarrelTrace };

	// Debug and Effect
	PlayFireSound();
	PlayFireVFX(BarrelEnd);
	DrawDebugLine(GetWorld(), BarrelStart, BarrelEnd, FColor::Red, false, 3.f);
	if (FireHit.bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), FireHit.ImpactPoint, 15.f, 24, FColor::Green, false, 3.0f);
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


void AWeapon::PlayReloadSound()
{
	if (ReloadSound)
	{
		UGameplayStatics::PlaySound2D(this, ReloadSound);
	}
}

void AWeapon::SetUpWeaponProperties(EWeaponState State)
{
	switch (State)
	{
	case EWeaponState::EWS_OnGround:
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetVisibility(true);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionBox->SetSimulatePhysics(true);
		CollisionBox->SetEnableGravity(true);
		CollisionBox->SetVisibility(true);
		break;
	case EWeaponState::EWS_Picking:
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetVisibility(true);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetSimulatePhysics(false);
		CollisionBox->SetEnableGravity(false);
		CollisionBox->SetVisibility(true);
		break;
	case EWeaponState::EWS_Equipped:
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetVisibility(true);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetSimulatePhysics(false);
		CollisionBox->SetEnableGravity(false);
		CollisionBox->SetVisibility(true);
		break;
	case EWeaponState::EWS_InBackpack:
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetVisibility(false);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetSimulatePhysics(false);
		CollisionBox->SetEnableGravity(false);
		CollisionBox->SetVisibility(false);
		break;
	}
}

