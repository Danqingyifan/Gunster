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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float Health;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	void Die();
	void PlayHitMontage(float PlayRate = 1.f);
	void PlayDeathMontage(float PlayRate = 1.f);

	UFUNCTION(BlueprintCallable)
	void StoreHitWidget(class UUserWidget* HitWidget, FVector HitLocation);
	void RemoveHitWidget(class UUserWidget* HitWidget);
	void UpdateHitWidgetLocation();

	FVector TransformLocalToWorld(FVector LocalLocation);

	UFUNCTION()
	void OnAgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
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
private:
	FTimerHandle HealthBarTimer;
	float HealthBarDisplayTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimInstance", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* BulletHitMontage;
	FTimerHandle HitReactTimer;
	bool bCanReactToHit;
	TMap<class UUserWidget*, FVector> HitAmountWidgetsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIBehavior", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	// MakeEditWidget = "true" make the variable become relative(local space) to the Enemy(transform),and visible in the editor
	FVector PatrolPoint;

	class AEnemyAIController* EnemyAIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AgroSphere", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsStunned;
	float StunProbability;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

};
