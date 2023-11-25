// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GunsterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"

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

void AGunsterPlayerController::SetupInputComponent()
{

}



