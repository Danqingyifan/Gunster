// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../Gunster.h"
#include "Items/Weapon/Weapon.h"
#include "Player/GunsterCharacter.h"
#include "Player/GunsterPlayerController.h"
#include "Enemy/Enemy.h"

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
	:bShouldFire(false), bCanFire(true), FireState(EFireState::ECS_Unoccupied) // Fire Params Init
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
	WeaponAmmo = ClipCapacity;
	FireState = EFireState::ECS_Unoccupied;
	WeaponOwner = Cast<AGunsterCharacter>(GetOwner());
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


// note that every variable you change OnConstruction() must be UPROPERTY()
// otherwise it will be defaulted on BeginPlay();
void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString WeaponDataTablePath{ TEXT("/Script/Engine.DataTable'/Game/Gunster/Weapon/DataTable/DT_WeaponDataTable.DT_WeaponDataTable'") };
	UDataTable* DataTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponDataTablePath));
	if (DataTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_Pistol:
			WeaponDataRow = DataTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));
			break;
		case EWeaponType::EWT_Shotgun:
			WeaponDataRow = DataTableObject->FindRow<FWeaponDataTable>(FName("ShotGun"), TEXT(""));
			break;
		case EWeaponType::EWT_SMG:
			WeaponDataRow = DataTableObject->FindRow<FWeaponDataTable>(FName("SMG"), TEXT(""));
			break;
		case EWeaponType::EWT_Rifle:
			WeaponDataRow = DataTableObject->FindRow<FWeaponDataTable>(FName("Rifle"), TEXT(""));
			break;
		case EWeaponType::EWT_MAX:
			UE_LOG(LogTemp, Display, TEXT("WeaponType NOT SET"));
			break;
		}

		if (WeaponDataRow)
		{
			WeaponName = WeaponDataRow->WeaponName;
			StartingAmmo = WeaponDataRow->StartingAmmo;
			ClipCapacity = WeaponDataRow->ClipCapacity;
			FireRate = WeaponDataRow->FireRate;
			ReloadTime = WeaponDataRow->ReloadTime;
			BaseDamage = WeaponDataRow->BaseDamage;
			HeadShotMultiplier = WeaponDataRow->HeadShotMultiplier;
			WeaponIcon = WeaponDataRow->WeaponIcon;
			AmmoTypeIcon = WeaponDataRow->AmmoTypeIcon;
			SetFireSound(WeaponDataRow->FireSound); 
			SetSmokeTrail(WeaponDataRow->SmokeTrail);
			SetWeaponMesh(WeaponDataRow->WeaponMesh);
		}
	}
}

// Implementation

//Weapon Fire
void AWeapon::Fire()
{
	if (bCanFire && FireState == EFireState::ECS_Unoccupied)
	{
		FireState = EFireState::ECS_Firing;
		bCanFire = false;
		if (WeaponAmmo > 0)
		{
			TrackTrajectory();
			//Using Lambda Delegate
			GetWorld()->GetTimerManager().SetTimer
			(
				TimerHandle,
				([this]() {
					bCanFire = true;
					FireState = EFireState::ECS_Unoccupied;
					}),
				FireRate,
				false
			);
			WeaponAmmo--;
		}
		else
		{
			WeaponOwner = Cast<AGunsterCharacter>(GetOwner());
			if (WeaponOwner)
			{
				WeaponOwner->Reload();
			}
			PlayOutOfAmmoSound();
		}
	}
}

FHitResult AWeapon::TrackTrajectory()
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
	FVector CrossHairEnd{ CrossHairStart + CrossHairWorldDirection * 3'000 };
	GetWorld()->LineTraceSingleByChannel(CrossHairTrace, CrossHairStart, CrossHairEnd, ECC_PlayerWeaponChannel);
	if (CrossHairTrace.bBlockingHit)
	{
		CrossHairEnd = CrossHairTrace.ImpactPoint;
	}

	FHitResult BarrelTrace;
	FVector BarrelStart{ MuzzlePosition->GetComponentTransform().GetLocation() };
	FVector BarrelEnd{ CrossHairEnd + (CrossHairTrace.ImpactPoint - BarrelStart).GetSafeNormal() * 10.f };
	// Need one extra forward vector to make sure the bullet will hit the target

	GetWorld()->LineTraceSingleByChannel(BarrelTrace, BarrelStart, BarrelEnd, ECC_PlayerWeaponChannel);

	FHitResult FireHit{ BarrelTrace };

	// Debug and Effect
	PlayFireSound();
	PlayFireVFX(BarrelEnd);
	DrawDebugLine(GetWorld(), BarrelStart, BarrelEnd, FColor::Red, false, 3.f);
	if (FireHit.bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), FireHit.ImpactPoint, 15.f, 24, FColor::Green, false, 3.0f);
		if (IOnBulletHitInterface* BulletHitInterface = Cast<IOnBulletHitInterface>(FireHit.GetActor())) // Check if the hit actor is a valid target
		{
			BulletHitInterface->OnBulletHit_Implementation(FireHit);
			if (AEnemy* HitEnemy = Cast<AEnemy>(FireHit.GetActor()))
			{
				float Damage;
				bool bIsHeadShot;
				WeaponOwner = Cast<AGunsterCharacter>(GetOwner());
				if (WeaponOwner)
				{
					if (FireHit.BoneName == "head")
					{
						UGameplayStatics::ApplyDamage(HitEnemy, BaseDamage * HeadShotMultiplier, WeaponOwner->GetController(), WeaponOwner, UDamageType::StaticClass());
						Damage = BaseDamage * HeadShotMultiplier;
						bIsHeadShot = true;
					}
					else
					{
						UGameplayStatics::ApplyDamage(HitEnemy, BaseDamage, WeaponOwner->GetController(), WeaponOwner, UDamageType::StaticClass());
						Damage = BaseDamage;
						bIsHeadShot = false;
					}
					HitEnemy->ShowHitDamageAmount(Damage, FireHit.Location, bIsHeadShot);
				}

			}
		}
		else
		{
			PlayDefaultImpactVFX(FireHit);
		}

	}
	return FireHit;
}

void AWeapon::ReloadMagazine()
{
	if (StartingAmmo > 0 && WeaponAmmo < ClipCapacity && FireState != EFireState::ECS_Reloading)
	{
		bCanFire = false;
		FireState = EFireState::ECS_Reloading;
		WeaponOwner = Cast<AGunsterCharacter>(GetOwner());
		if (WeaponOwner)
		{
			WeaponOwner->bIsReloading = true;
		}

		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle,
			([this]() {
				bCanFire = true;
				FireState = EFireState::ECS_Unoccupied;
				if (WeaponOwner = Cast<AGunsterCharacter>(GetOwner()))
				{
					WeaponOwner->bIsReloading = false;
				}
				if (StartingAmmo >= (ClipCapacity - WeaponAmmo))
				{
					StartingAmmo -= (ClipCapacity - WeaponAmmo);
					WeaponAmmo = ClipCapacity;
				}
				else
				{
					WeaponAmmo = WeaponAmmo + StartingAmmo;
					StartingAmmo = 0;
				}
				}),
			ReloadTime,
			false
		);
		PlayReloadSound();
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

void AWeapon::PlayDefaultImpactVFX(FHitResult& HitResult)
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

void AWeapon::PlayOutOfAmmoSound()
{
	if (OutOfAmmoSound)
	{
		UGameplayStatics::PlaySound2D(this, OutOfAmmoSound);
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

