// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/Enemy.h"

#include "../Gunster.h"
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
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AEnemy::AEnemy()
	:MaxHealth(100.f), HealthBarDisplayTime(3.f), bCanReactToHit(true), bIsStunned(false),
	StunProbability(0.2f), bIsInCombatRange(false), bIsDead(false), WeaponDamage(50.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroRangeSphere"));
	AgroRangeSphere->SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	//SetWeaponCollisionVolume(LeftWeaponCollisionVolume, TEXT("LeftWeaponCollisionVolume"), TEXT("LeftWeaponCollisionSocket"));
	//SetWeaponCollisionVolume(RightWeaponCollisionVolume, TEXT("RightWeaponCollisionVolume"), TEXT("RightWeaponCollisionSocket"));

	LeftWeaponCollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollisionVolume"));
	LeftWeaponCollisionVolume->SetupAttachment(GetMesh(), TEXT("LeftWeaponCollisionSocket"));
	RightWeaponCollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollisionVolume"));
	RightWeaponCollisionVolume->SetupAttachment(GetMesh(), TEXT("RightWeaponCollisionSocket"));

	//Orient to Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	//Arrow Component is the real direction of the character facing
	//Remenber to adjust the direction of the character to be identical to Arrow Component  in the blueprint

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	//Make the AI Controller can possess the dynamically spawned enemy
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//delegate
	// Function Binded must be UFUNCTION, otherwise, it will not be called
	// better than put in beginplay,it run before beginplay
	AgroRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAgroSphereOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnCombatRangeSphereOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnCombatRangeSphereEndOverlap);
	LeftWeaponCollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnWeaponCollisionSphereOverlap);
	RightWeaponCollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnWeaponCollisionSphereOverlap);

	//AI
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePawn);

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
	SetWeaponCollisionVolume(LeftWeaponCollisionVolume);
	SetWeaponCollisionVolume(RightWeaponCollisionVolume);
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

void AEnemy::OnBulletHit_Implementation(const FHitResult& HitResult)
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
	if (HitResult.BoneName == "head")
	{
		if (FMath::FRandRange(0.f, 1.f) <= StunProbability)
		{
			SetStunned(true);
			PlayHitMontage();
		}
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), DamageCauser);
	}

	if (Health - DamageAmount <= 0)
	{
		Health = 0;
		if (!bIsDead)
		{
			SetDead(true);
			Die();
		}
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

void AEnemy::Die()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->GetBrainComponent()->StopLogic("Dead");
	}
	HideHealthBar();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECC_PlayerWeaponChannel, ECollisionResponse::ECR_Ignore);
	

	//Two Ways to play death animation
	
	//Use Montage
	
	PlayDeathMontage();

	//Use Ragdoll
	
	//GetMesh()->SetAllBodiesSimulatePhysics(true);
	//GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

}
void AEnemy::SetDead(bool Dead)
{
	bIsDead = Dead;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), bIsDead);
	}
}
void AEnemy::DeathFinish()
{
	HideBossHealthBar();
	GetMesh()->bPauseAnims = true;
	FTimerHandle DeathTimer;
	GetWorld()->GetTimerManager().SetTimer(DeathTimer, ([this]() { Destroy(); }), 2.f, false);
}


void AEnemy::ActivateCollision(class UBoxComponent* WeaponCollisionVolume)
{
	WeaponCollisionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision(class UBoxComponent* WeaponCollisionVolume)
{
	WeaponCollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::PlayAttackSound()
{
	if (WeaponAttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponAttackSound, GetActorLocation());
	}
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AEnemy, MaxHealth);
	DOREPLIFETIME(AEnemy, Health);
}

void AEnemy::SetWeaponCollisionVolume(class UBoxComponent* WeaponCollisionVolume)
{
	WeaponCollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollisionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollisionVolume->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
}

void AEnemy::ShowHealthBar_Implementation()
{
	if (Health > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthBarTimer);
		GetWorld()->GetTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime, false);
	}
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
	//Reserved
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

void AEnemy::OnWeaponCollisionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto GunsterCharacter = Cast<AGunsterCharacter>(OtherActor))
	{
		if (GunsterCharacter->GetMeleeImpactSound())
		{
			UGameplayStatics::PlaySoundAtLocation(this, GunsterCharacter->GetMeleeImpactSound(), GetActorLocation());
		}
		UGameplayStatics::ApplyDamage(GunsterCharacter, WeaponDamage, EnemyAIController, this, UDamageType::StaticClass());
	}
}

void AEnemy::OnSeePawn(APawn* Pawn)
{
	if (Pawn == nullptr) return;

	if (auto GunsterCharacter = Cast<AGunsterCharacter>(Pawn))
	{
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), GunsterCharacter);
			DrawDebugString(GetWorld(), GetActorLocation(), "Player Spotted", nullptr, FColor::Red, 2.f);
		}
	}
}
