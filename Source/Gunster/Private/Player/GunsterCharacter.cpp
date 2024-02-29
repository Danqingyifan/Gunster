// Copyright Epic Games, Inc. All Rights Reserved.

//MyClass
#include "Player/GunsterCharacter.h"
#include "Player/GunsterPlayerController.h"
#include "Items/Weapon/Weapon.h"
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
#include "Kismet/GameplayStatics.h"

AGunsterCharacter::AGunsterCharacter()
	:bIsAiming(false), bIsShooting(false), bIsReloading(false), IdleFOV(100.f), AimFOV(50.f), MaxHealth(100)
{
	SetUpCamera();
	SetUpControllerRotation();
	SetUpCharacterMovement();
}

void AGunsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeapon();
	Health = MaxHealth;

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
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
	const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName("hand_rSocket");
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

			EnhancedInputComponent->BindAction(GunsterController->SprintAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Sprint);

			EnhancedInputComponent->BindAction(GunsterController->ReloadAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Reload);

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
		AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage && EquippedWeapon->GetCanReload())
		{
			EquippedWeapon->ReloadMagazine();
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(FName("Reload SMG"));
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

}

void AGunsterCharacter::Dash()
{

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
		FActorSpawnParameters SpawnInfo;
		FTransform LeftSocketTransform = GetMesh()->GetSocketTransform(Socket->SocketName);
		FVector SpawnLocation = LeftSocketTransform.GetLocation();
		FRotator SpawnRotation = LeftSocketTransform.GetRotation().Rotator();
		return GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnLocation, SpawnRotation, SpawnInfo);
	}
	return nullptr;
}

void AGunsterCharacter::AttachWeapon(AWeapon* Weapon, const USkeletalMeshSocket* Socket)
{
	if (Weapon)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket->SocketName);
		Weapon->SetOwner(this);
		//ignore the collision of HoldingWeapon against the Character
		Weapon->SetActorEnableCollision(false);
	}
}

float AGunsterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

void AGunsterCharacter::OnBulletHit_Implementation(FHitResult HitResult)
{

}

void AGunsterCharacter::Die()
{

}