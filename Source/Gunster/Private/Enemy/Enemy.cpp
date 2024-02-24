// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/Enemy.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AEnemy::AEnemy()
	:MaxHealth(100.f), HealthBarDisplayTime(3.f), bCanReactToHit(true)
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
	UpdateHitWidgetLocation();
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
	PlayHitMontage();
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
	PlayDeathMontage();
	FTimerHandle MontagePauseTimer, DestroyTimer;
	GetWorld()->GetTimerManager().SetTimer(MontagePauseTimer, ([this]() { GetMesh()->GetAnimInstance()->Montage_Pause(); }), 1.25f, false);
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer, ([this]() { Destroy(); }), 2.f, false);
}

void AEnemy::PlayHitMontage(float PlayRate /*= 1.f*/)
{
	if (bCanReactToHit)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(BulletHitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection("HitReactFront", BulletHitMontage);
		}
		bCanReactToHit = false;
		GetWorld()->GetTimerManager().SetTimer(HitReactTimer, ([this]() { bCanReactToHit = true; }), FMath::FRandRange(0.5f, 1.f), false);
	}
}

void AEnemy::PlayDeathMontage(float PlayRate /*= 1.f*/)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(BulletHitMontage, PlayRate);
		AnimInstance->Montage_JumpToSection("DeathToBack", BulletHitMontage);
	}
}

void AEnemy::StoreHitWidget(UUserWidget* HitWidget, FVector HitLocation)
{
	HitAmountWidgetsMap.Add(HitWidget, HitLocation);
	// Then remove them
	FTimerHandle RemoveHitWidgetTimer;
	GetWorld()->GetTimerManager().SetTimer(RemoveHitWidgetTimer,
		([this, HitWidget]()
			{
				RemoveHitWidget(HitWidget);
			}),
		1.5f, false);

}

void AEnemy::RemoveHitWidget(class UUserWidget* HitWidget)
{
	HitAmountWidgetsMap.Remove(HitWidget);
	HitWidget->RemoveFromParent();
}

void AEnemy::UpdateHitWidgetLocation()
{
	for (auto& HitWidgetPair : HitAmountWidgetsMap)
	{

		UUserWidget* HitWidget{ HitWidgetPair.Key };
		FVector HitLocation{ HitWidgetPair.Value };

		FVector2D ScreenLocation;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), HitLocation, ScreenLocation);
		HitWidget->SetPositionInViewport(ScreenLocation);
	}
}

