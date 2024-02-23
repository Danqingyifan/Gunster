// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GunsterPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Blueprint/UserWidget.h"

#include "Player/GunsterCharacter.h"
#include "GameFramework/Character.h"

AGunsterPlayerController::AGunsterPlayerController()
{

}

void AGunsterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetupInputComponent();
	SetUpCrossHair();
	LimitCameraView();
	DrawHUDOverlayToScreen();
}

void AGunsterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateCrossHairSpread(DeltaTime);
	//DrawHUD
}

void AGunsterPlayerController::SetupInputComponent()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(GravesMappingContext, 0);
	}
}

void AGunsterPlayerController::SetUpCrossHair()
{
	CrossHairSpreadBase = 4.0f;
	CrossHairSpreadMultiplier = 1.0f;
	CrossHairVelocityFactor = 1.0f;
	CrossHairAimFactor = 1.0f;
	CrossHairShootingFactor = 1.0f;
}

void AGunsterPlayerController::CalculateCrossHairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.0f, 250.0f };
	FVector2D VelocityMultiplierRange{ 1.0f, 3.0f };

	if (ACharacter* OwningCharacter = GetCharacter())
	{
		if (AGunsterCharacter* GunsterCharacter = Cast<AGunsterCharacter>(OwningCharacter))
		{
			//Velocity
			CrossHairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, GunsterCharacter->GetVelocity().Size());

			//Aiming
			if (GunsterCharacter->GetIsAiming())
			{
				CrossHairAimFactor = FMath::FInterpTo(CrossHairAimFactor, 0.7f, GetWorld()->GetDeltaSeconds(), 30.0f);
			}
			else
			{
				CrossHairAimFactor = FMath::FInterpTo(CrossHairAimFactor, 1.0f, GetWorld()->GetDeltaSeconds(), 30.0f);
			}

			//Shooting
			if (GunsterCharacter->GetIsShooting())
			{
				CrossHairShootingFactor = FMath::FInterpTo(CrossHairShootingFactor, 5.0f, GetWorld()->GetDeltaSeconds(), 30.0f);
			}
			else
			{
				CrossHairShootingFactor = FMath::FInterpTo(CrossHairShootingFactor, 1.0f, GetWorld()->GetDeltaSeconds(), 30.0f);
			}
		}
	}
	CrossHairSpreadMultiplier = 1.0f * CrossHairAimFactor * CrossHairVelocityFactor * CrossHairShootingFactor;
}

float AGunsterPlayerController::GetCrossHairSpreadMultiplier() const
{
	return CrossHairSpreadMultiplier;
}

bool AGunsterPlayerController::DeprojectCrossHairToWorld(FVector& CrossHairWorldPosition, FVector& CrossHairWorldDirection)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrossHairScreenLocation{ ViewportSize.X / 2 ,ViewportSize.Y / 2 };
	CrossHairScreenLocation.X += 30.0f;
	CrossHairScreenLocation.Y -= 20.0f;

	return UGameplayStatics::DeprojectScreenToWorld
	(
		this,
		CrossHairScreenLocation,
		CrossHairWorldPosition,
		CrossHairWorldDirection
	);
}

// Prepared for Aiming
void AGunsterPlayerController::LimitCameraView()
{
	APlayerCameraManager* Manager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	Manager->ViewPitchMax = 70.0f;
	Manager->ViewPitchMin = -70.0f;
}

void AGunsterPlayerController::DrawHUDOverlayToScreen()
{
	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this,HUDOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

