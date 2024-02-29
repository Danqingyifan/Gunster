// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnBulletHitInterface.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class GUNSTER_API AEnemy : public ACharacter, public IOnBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "HitEffects")
	class USoundCue* HitSound;
	UPROPERTY(EditAnywhere, Category = "HitEffects")
	class UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	FName EnemyName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float Health;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowBossHealthBar();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void HideBossHealthBar();

	void Die();
	void PlayHitMontage(float PlayRate = 1.f);
	void PlayDeathMontage(float PlayRate = 1.f);
	UFUNCTION(BlueprintCallable)
	void PlayCombatMontage(FName Section, float PlayRate = 1.f);
	UFUNCTION(BlueprintPure)
	FName GetCombatSection();

	UFUNCTION(BlueprintCallable)
	void StoreHitWidget(class UUserWidget* HitWidget, FVector HitLocation);
	void RemoveHitWidget(class UUserWidget* HitWidget);
	void UpdateHitWidgetLocation();

	FVector TransformLocalToWorld(FVector LocalLocation);

	UFUNCTION()
	void OnAgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCombatRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCombatRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnWeaponCollisionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnBulletHit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitDamageAmount(float Damage, FVector HitLocation, bool bIsHeadShot);

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION(BlueprintCallable)
	void SetDead(bool Dead);
	UFUNCTION(BlueprintCallable)
	void DeathFinish();

	UFUNCTION(BlueprintCallable)
	void ActivateCollision(class UBoxComponent* WeaponCollisionVolume);
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision(class UBoxComponent* WeaponCollisionVolume);

	UFUNCTION(BlueprintCallable)
	void PlayAttackSound();
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimInstance", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* BulletHitMontage;
	FTimerHandle HealthBarTimer;
	float HealthBarDisplayTime;
	FTimerHandle HitReactTimer;
	bool bCanReactToHit;
	TMap<class UUserWidget*, FVector> HitAmountWidgetsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIBehavior", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	// MakeEditWidget = "true" make the variable become relative(local space) to the Enemy(transform),and visible in the editor
	FVector PatrolPoint;

	class AEnemyAIController* EnemyAIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AgroSphere", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroRangeSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsStunned;
	float StunProbability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CombatRangeSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInCombatRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimInstance", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* CombatMontage;

	void SetWeaponCollisionVolume(class UBoxComponent* WeaponCollisionVolume);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LeftWeaponCollisionVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RightWeaponCollisionVolume;
	float WeaponDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* WeaponAttackSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* VocalAttackSound;
};
