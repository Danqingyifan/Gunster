// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GunsterPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"

AGunsterPlayerController::AGunsterPlayerController()
{

}

void AGunsterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(GravesMappingContext, 0);
	}

}

void AGunsterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGunsterPlayerController::SetupInputComponent()
{

}

bool AGunsterPlayerController::DeprojectCrossHairToWorld(FVector& CrossHairWorldPosition, FVector& CrossHairWorldDirection)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrossHairPosition{ ViewportSize.X / 2,ViewportSize.Y / 2 };

	return UGameplayStatics::DeprojectScreenToWorld
	(
		UGameplayStatics::GetPlayerController(this, 0),
		CrossHairPosition,
		CrossHairWorldPosition,
		CrossHairWorldDirection
	);
}


