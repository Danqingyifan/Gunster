// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"
#include "Player/GunsterCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
	:MaxHealth(100.f), HealthBarDisplayTime(3.f), bCanReactToHit(true), bIsStunned(false),
	StunProbability(0.2f), bIsDead(false), bIsInCombatRange(false)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroRangeSphere"));
	AgroRangeSphere->SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());



	//Orient to Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	//Arrow Component is the real direction of the character facing
	//Remenber to adjust the direction of the character to be identical to Arrow Component  in the blueprint
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	//ignore camera
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//BUG::Can't detect the GunsterCharacter after adding below
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	FVector WorldPatrolPoint = TransformLocalToWorld(PatrolPoint);
	// Set the AI Controller
	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyAIController->RunBehaviorTree(BehaviorTree);
	}

	// Function Binded must be UFUNCTION, otherwise, it will not be called
	AgroRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAgroSphereOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnCombatRangeSphereOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnCombatRangeSphereEndOverlap);
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

	// determine if be Stunned
	if (FMath::FRandRange(0.f, 1.f) <= StunProbability)
	{
		SetStunned(true);
		PlayHitMontage();
	}
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

void AEnemy::SetStunned(bool Stunned)
{
	bIsStunned = Stunned;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsStunned"), bIsStunned);
	}
}

void AEnemy::SetDead(bool Dead)
{
	bIsDead = Dead;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), bIsDead);
	}
}

void AEnemy::SetWeaponCollisionSphere(USphereComponent* WeaponCollisionSphere, FName SocketName)
{
	WeaponCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LeftWeaponCollisionSphere"));
	WeaponCollisionSphere->SetupAttachment(GetMesh(), "LeftWeaponSocket");
	WeaponCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
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
	HideBossHealthBar();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayDeathMontage();
	SetDead(true);
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

void AEnemy::PlayCombatMontage(FName Section, float PlayRate /*= 1.f*/)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, CombatMontage);
	}
}

FName AEnemy::GetCombatSection()
{
	FName SectionName;
	const int32 SectionPick = FMath::RandRange(1, 2);
	switch (SectionPick)
	{
	case 1:
		SectionName = "ComboL";
		break;
	case 2:
		SectionName = "ComboR";
		break;
	}
	return SectionName;
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

FVector AEnemy::TransformLocalToWorld(FVector LocalLocation)
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), LocalLocation);
}

void AEnemy::OnAgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	if (auto Target = Cast<AGunsterCharacter>(OtherActor))
	{
		ShowBossHealthBar();
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Target);
		}
	}
}

void AEnemy::OnAgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HideBossHealthBar();
}

void AEnemy::OnCombatRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	if (auto Target = Cast<AGunsterCharacter>(OtherActor))
	{
		bIsInCombatRange = true;
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsInCombatRange"), bIsInCombatRange);
		}
	}
}

void AEnemy::OnCombatRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	if (auto Target = Cast<AGunsterCharacter>(OtherActor))
	{
		bIsInCombatRange = false;
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsInCombatRange"), bIsInCombatRange);
		}
	}
}

