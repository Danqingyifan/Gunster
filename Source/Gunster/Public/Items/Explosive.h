// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnBulletHitInterface.h"
#include "Items/Item.h"
#include "GameFramework/Actor.h"

#include "Explosive.generated.h"

UCLASS()
class GUNSTER_API AExplosive : public AActor, public IOnBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplosive();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnBulletHit_Implementation(const FHitResult& HitResult) override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCExplode(const FHitResult& HitResult);

	UFUNCTION(Client, Unreliable)
	void ClientRPCAddRotate();

	UFUNCTION(Server, Unreliable)
	void ServerRPCExplode(const FHitResult& HitResult);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//RepNotify
	UPROPERTY(ReplicatedUsing = "OnRep_Explode", BlueprintReadWrite)
	bool bExploded;

	UFUNCTION()
	void OnRep_Explode();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ImpactEffects", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ExplosionSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ImpactEffects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ExplosionParticles;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ImpactEffects", meta = (AllowPrivateAccess = "true"))
	class URadialForceComponent* RadialForce;

};
