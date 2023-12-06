// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GunsterPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Character.h"

AGunsterPlayerController::AGunsterPlayerController()
{

}

void AGunsterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetupInputComponent();
}

void AGunsterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LimitCameraView();
	//DrawHUD
}

void AGunsterPlayerController::SetupInputComponent()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(GravesMappingContext, 0);
	}
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
	float CharacterOrientation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorForwardVector().Rotation().Yaw;

	Manager->ViewPitchMax = 60.0f;
	Manager->ViewPitchMin = -60.0f;
	Manager->ViewYawMax = CharacterOrientation + 95.0f;
	Manager->ViewYawMin = CharacterOrientation - 95.0f;
}


