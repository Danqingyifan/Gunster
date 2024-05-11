// Copyright Epic Games, Inc. All Rights Reserved.

//MyClass
#include "Player/GunsterCharacter.h"
#include "../Gunster.h"
#include "Player/GunsterPlayerController.h"
#include "Items/Weapon/Weapon.h"
#include "Enemy/EnemyAIController.h"
#include "InteractableComponent.h"
#include "CharacterAttributesComponent.h"
#include "Action/GunsterActionComponent.h"
//Camera
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
// Character
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
//Net
#include "Net/UnrealNetwork.h"
#include "Player/GunsterPlayerState.h"
#include "AbilitySystemComponent.h"

AGunsterCharacter::AGunsterCharacter()
	:bIsAiming(false), bIsShooting(false), bIsReloading(false), IdleFOV(100.f), AimFOV(50.f), MaxHealth(100)
{
	SetUpCamera();
	SetUpControllerRotation();
	SetUpCharacterMovement();

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributesComponent>(TEXT("CharacterAttributesComponent"));
	GunsterActionComponent = CreateDefaultSubobject<UGunsterActionComponent>(TEXT("GunsterActionComponent"));

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AGunsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeapon();
	Health = MaxHealth;

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//Set up the controller
	if (AGunsterPlayerController* GunsterController = Cast<AGunsterPlayerController>(Controller))
	{
		GunsterPlayerController = GunsterController;
	}
}

void AGunsterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	SetUpInput(PlayerInputComponent);
}

void AGunsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Zoom Camera if Aim 
	ZoomIfAim(bIsAiming, DeltaTime);
}


//Init Section
//Constructor
void AGunsterCharacter::SetUpControllerRotation()
{
	//if use Controller Rotate
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AGunsterCharacter::SetUpCamera()
{
	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Set Default FOV
	FollowCamera->FieldOfView = IdleFOV;
}

void AGunsterCharacter::SetUpCharacterMovement()
{
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	//CharacterMovement Argument Setting
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

//BeginPlay
void AGunsterCharacter::SpawnDefaultWeapon()
{
	const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName("hand_lSocket");
	// Spawn the default weapon(Dual_SMG) at the location of the hand socket

	if (DefaultWeaponClass && RightHandSocket)
	{
		EquippedWeapon = SpawnWeapon(RightHandSocket, DefaultWeaponClass);
		//RightHoldingWeapon = SpawnWeapon(RightHandSocket, DefaultWeaponClass);

		if (EquippedWeapon)
		{
			AttachWeapon(EquippedWeapon, RightHandSocket);
		}
	}

}

void AGunsterCharacter::SetUpInput(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (AGunsterPlayerController* GunsterController = Cast<AGunsterPlayerController>(Controller))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
		{
			//No Jump Action For Graves

			EnhancedInputComponent->BindAction(GunsterController->MoveAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Move);

			EnhancedInputComponent->BindAction(GunsterController->LookAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Look);

			EnhancedInputComponent->BindAction(GunsterController->TriggerAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::PullTrigger);
			EnhancedInputComponent->BindAction(GunsterController->TriggerAction, ETriggerEvent::Completed, this, &AGunsterCharacter::ReleaseTrigger);

			EnhancedInputComponent->BindAction(GunsterController->AimAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Aim);
			EnhancedInputComponent->BindAction(GunsterController->AimAction, ETriggerEvent::Completed, this, &AGunsterCharacter::StopAim);

			EnhancedInputComponent->BindAction(GunsterController->DodgeAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Dodge);

			EnhancedInputComponent->BindAction(GunsterController->SprintAction, ETriggerEvent::Started, this, &AGunsterCharacter::Sprint);
			EnhancedInputComponent->BindAction(GunsterController->SprintAction, ETriggerEvent::Completed, this, &AGunsterCharacter::StopSprint);


			EnhancedInputComponent->BindAction(GunsterController->ReloadAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Reload);

			EnhancedInputComponent->BindAction(GunsterController->InteractAction, ETriggerEvent::Started, this, &AGunsterCharacter::Interact);

			EnhancedInputComponent->BindAction(GunsterController->SwitchWeaponAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::SwitchWeapon);
			EnhancedInputComponent->BindAction(GunsterController->SwitchWeaponAction, ETriggerEvent::Completed, this, &AGunsterCharacter::FinishSwitchWeapon);
		}
	}
}

//Input Action
void AGunsterCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGunsterCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGunsterCharacter::PullTrigger()
{

	GunsterActionComponent->StartActionByName(this, "PullTrigger");


	if (EquippedWeapon && bIsAiming && EquippedWeapon->GetCanFire())
	{
		bIsShooting = true;
		EquippedWeapon->StartFire();
		AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && StrafeMontage)
		{
			AnimInstance->Montage_Play(StrafeMontage);
			AnimInstance->Montage_JumpToSection(FName("Strafe"));
		}
	}
}

void AGunsterCharacter::ReleaseTrigger()
{

	GunsterActionComponent->StopActionByName(this, "PullTrigger");

	if (EquippedWeapon)
	{
		EquippedWeapon->StopFire();
		bIsShooting = false;
	}
}

void AGunsterCharacter::Reload()
{
	if (EquippedWeapon)
	{
		if (EquippedWeapon->GetCanReload())
		{
			AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && ReloadMontage)
			{
				AnimInstance->Montage_Play(ReloadMontage);
				AnimInstance->Montage_JumpToSection(FName("ReloadSMG"));
			}
			EquippedWeapon->ReloadMagazine();
		}
	}
}

void AGunsterCharacter::Aim()
{
	bIsAiming = true;
	AimLocomotion(bIsAiming);
}

void AGunsterCharacter::StopAim()
{
	bIsAiming = false;
	AimLocomotion(bIsAiming);
}

void AGunsterCharacter::ZoomIfAim(bool isAiming, float DeltaTime)
{
	if (bIsAiming)
	{
		ZoomCamera(AimFOV, DeltaTime);
	}
	else
	{
		ZoomCamera(IdleFOV, DeltaTime);
	}
}

void AGunsterCharacter::AimLocomotion(bool isAiming)
{
	if (isAiming)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->MaxWalkSpeed = 250.f;
	}
	else
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}

}

void AGunsterCharacter::ZoomCamera(float TargetFOV, float DeltaTime)
{
	float CurrentFOV = GetFollowCamera()->FieldOfView;
	if (CurrentFOV != TargetFOV)
	{
		float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 20.0f);
		GetFollowCamera()->SetFieldOfView(NewFOV);
	}
}

void AGunsterCharacter::Dodge()
{

}

void AGunsterCharacter::Sprint() 
{
	UE_LOG(LogTemp, Warning, TEXT("Sprint"));
	GunsterActionComponent->StartActionByName(this, "Sprint");
}

void AGunsterCharacter::StopSprint()
{
	UE_LOG(LogTemp, Warning, TEXT("StopSprint"));
	GunsterActionComponent->StopActionByName(this, "Sprint");
}

void AGunsterCharacter::Dash()
{

}
void AGunsterCharacter::Interact()
{
	if (InteractableComponent)
	{
		InteractableComponent->InteractWith();
	}
}

void AGunsterCharacter::SwitchWeapon()
{

}
void AGunsterCharacter::FinishSwitchWeapon()
{

}

//Implementation
class AWeapon* AGunsterCharacter::SpawnWeapon(const USkeletalMeshSocket* Socket, const TSubclassOf<class AWeapon> WeaponClass)
{
	if (Socket)
	{
		FActorSpawnParameters SpawnParams;

		//Saved for later use
		//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//SpawnParams.Instigator = this;

		FTransform LeftSocketTransform = GetMesh()->GetSocketTransform(Socket->SocketName);
		FVector SpawnLocation = LeftSocketTransform.GetLocation();
		FRotator SpawnRotation = LeftSocketTransform.GetRotation().Rotator();
		return GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnLocation, SpawnRotation, SpawnParams);
	}
	return nullptr;
}

void AGunsterCharacter::AttachWeapon(AWeapon* Weapon, const USkeletalMeshSocket* Socket)
{
	if (Weapon)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket->SocketName);
		Weapon->SetOwner(this);
		//Set weapon mesh's object type
		EquippedWeapon = Weapon;
		//ignore the collision of HoldingWeapon against the Character
		Weapon->SetActorEnableCollision(false);
	}
}

void AGunsterCharacter::OnBulletHit(const FHitResult& HitResult)
{

}

float AGunsterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CharacterAttributesComponent->ApplyHealthChange(-DamageAmount);
	if (Health - DamageAmount <= 0)
	{
		Health = 0;
		Die();
		if (auto EnemyController = Cast<AEnemyAIController>(EventInstigator))
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool("IsTargetDead", true);
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AGunsterCharacter::Die()
{
	DisableInput(GunsterPlayerController);
	if (bIsAiming)
	{
		bIsAiming = false;
	}
	if (auto AnimationInstance = GetMesh()->GetAnimInstance())
	{
		AnimationInstance->Montage_Play(DeathMontage);
		//PlayRate need to be set after Montage_Play
		AnimationInstance->Montage_SetPlayRate(DeathMontage, 0.8f);
		AnimationInstance->Montage_JumpToSection(FName("Death"));

		SetLifeSpan(5.f);
	}
}

void AGunsterCharacter::DeathFinish()
{
	GetMesh()->bPauseAnims = true;
}



//`````New Part`````````//

//execuate before the implementation of the function
//if true,will exe the implementation
//if false,the client will call the function will be disconnected from the server
bool AGunsterCharacter::ServerRPCSpawnBox_Validate(int arg)
{
	if (arg > 100)
		return false;
	else
		return true;
}

void AGunsterCharacter::ServerRPCSpawnBox_Implementation(int arg)
{
	if (HasAuthority())
	{
		if (!BoxMesh)
		{
			return;
		}
		AStaticMeshActor* StaticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
		if (StaticMeshActor)
		{
			//set replicate
			StaticMeshActor->SetReplicates(true);
			StaticMeshActor->SetReplicateMovement(true);
			StaticMeshActor->SetMobility(EComponentMobility::Movable);

			StaticMeshActor->SetActorLocation(GetActorLocation() + FVector(0, 0, 100));

			StaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(BoxMesh);
			StaticMeshActor->GetStaticMeshComponent()->SetIsReplicated(true);
			StaticMeshActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
		}
	}

}


UAbilitySystemComponent* AGunsterCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}



void AGunsterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//Init ability actor info for the Server
	InitAbilityActorInfo();
}

void AGunsterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();

}

void AGunsterCharacter::InitAbilityActorInfo()
{
	AGunsterPlayerState* GunsterPlayerState = GetPlayerState<AGunsterPlayerState>();
	check(GunsterPlayerState);
	GunsterPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(GunsterPlayerState, this);
	AbilitySystemComponent = GunsterPlayerState->GetAbilitySystemComponent();
	AttributeSet = GunsterPlayerState->GetAttributeSet();
}

FVector AGunsterCharacter::GetPawnViewLocation() const
{
	if (FollowCamera)
	{
		return FollowCamera->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}
