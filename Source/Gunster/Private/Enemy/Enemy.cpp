// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/Enemy.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimMontage.h"
// Sets default values
AEnemy::AEnemy()
	:MaxHealth(100.f), HealthBarDisplayTime(3.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnBulletHit_Implementation(FHitResult HitResult)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.Location, FRotator(0), true);
	}
	ShowHealthBar();
	PlayMontageBySection(BulletHitMontage, FName("HitReactFront"), 1.0f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health - DamageAmount <= 0)
	{
		Health = 0;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AEnemy::ShowHealthBar_Implementation()
{
	if (Health > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthBarTimer);
		GetWorld()->GetTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime, false);
	}
}

void AEnemy::Die()
{
	HideHealthBar();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayMontageBySection(BulletHitMontage, FName("DeathToBack"), 1.f);
	GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
}

void AEnemy::PlayMontageBySection(UAnimMontage* Montage, FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Montage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, Montage);
	}
}

