// Fill out your copyright notice in the Description page of Project Settings.
#include "Items/Explosive.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"



// Sets default values
AExplosive::AExplosive()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->SetupAttachment(RootComponent);
	RadialForce->bImpulseVelChange = true;

	//Default constructor already add 4 object type to affect
	//RadialForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_PhysicsBody);

	bExploded = false;
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AExplosive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExplosive, bExploded);
}

//RepNotify
void AExplosive::OnBulletHit_Implementation(const FHitResult& HitResult)
{
	bExploded = true;
	if (HasAuthority())
	{
		MulticastRPCExplode(HitResult);
	}

	if (RadialForce)
	{
		//Using Movement Replicate to sync,so no need to call in RPC
		RadialForce->FireImpulse();
	}

	//Destroy after seconds
	FTimerHandle TimerHandle;
	

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, ([this]() { Destroy(); }), 2.0f, false);
}

void AExplosive::OnRep_Explode()  //RepNotify auto trigger for client but not for server
{
	if (bExploded)
	{

	}
}

//RPC
void AExplosive::MulticastRPCExplode_Implementation(const FHitResult& HitResult)
{
	//Only for cosmetic effect
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	if (ExplosionParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, HitResult.Location, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	}

	// If you destroy the actor immediately, the Multicast will not arrive at the client,so NOTHING gonna happen
	SetHidden(true);
	this->SetActorEnableCollision(false);



}

void AExplosive::ClientRPCAddRotate_Implementation()
{

}

void AExplosive::ServerRPCExplode_Implementation(const FHitResult& HitResult)
{

}




