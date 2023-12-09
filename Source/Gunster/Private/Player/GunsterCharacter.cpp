// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/GunsterCharacter.h"
#include "Player/GunsterPlayerController.h"
#include "Items/Weapon/Weapon.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/SkeletalMeshSocket.h"

AGunsterCharacter::AGunsterCharacter()
{
	InitConstructor();
}

void AGunsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeapon();
}

// Input
void AGunsterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
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

			EnhancedInputComponent->BindAction(GunsterController->DodgeAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Dodge);

			EnhancedInputComponent->BindAction(GunsterController->SprintAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Sprint);

			EnhancedInputComponent->BindAction(GunsterController->ReloadAction, ETriggerEvent::Triggered, this, &AGunsterCharacter::Reload);
		}
	}

}


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
	if (EquippedWeapon)
	{
		EquippedWeapon->StartFire();
	}
}

void AGunsterCharacter::ReleaseTrigger()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StopFire();
	}
}

void AGunsterCharacter::Dodge()
{

}

void AGunsterCharacter::Sprint()
{

}

void AGunsterCharacter::Reload()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ReloadMagazine();
	}
}


//
void AGunsterCharacter::SpawnDefaultWeapon()
{
	const class USkeletalMeshSocket* RightHandSocket =
		RightHandSocket = GetMesh()->GetSocketByName("hand_rSocket");

	const class USkeletalMeshSocket* LeftHandSocket =
		LeftHandSocket = GetMesh()->GetSocketByName("hand_lSocket");

	if (DefaultWeaponClass && LeftHandSocket && RightHandSocket)
	{
		FTransform SocketTransform = GetMesh()->GetSocketTransform(LeftHandSocket->SocketName);
		FActorSpawnParameters SpawnInfo;
		FVector SpawnLocation = SocketTransform.GetLocation();
		FRotator SpawnRotation = SocketTransform.GetRotation().Rotator();
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass, SpawnLocation, SpawnRotation, SpawnInfo);
		if (EquippedWeapon)
		{
			EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandSocket->SocketName);
			EquippedWeapon->SetOwner(this);
			//ignore the collision of EquippedWeapon
			EquippedWeapon->SetActorEnableCollision(false);
		}
	}
}


void AGunsterCharacter::InitConstructor()
{
	//if use Controller Rotate
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	//CharacterMovement Argument Setting
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}



