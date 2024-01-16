// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GunsterPlayerController.generated.h"

UCLASS()
class GUNSTER_API AGunsterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AGunsterPlayerController();
public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	void SetUpCrossHair();
	// IMC and IA Segment
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* GravesMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TriggerAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DodgeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrossHair", meta = (AllowPrivateAccess = "true"))
	class AHUD* crossHair;
public:
	//CrossHair
	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	float GetCrossHairSpreadMultiplier() const;

	void CalculateCrossHairSpread(float DeltaTime);

	bool DeprojectCrossHairToWorld(FVector& CrossHairWorldPosition, FVector& CrossHairWorldDirection);

	//Camera
	void LimitCameraView();
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrossHair", meta = (AllowPrivateAccess = "true"))
	class AHUD* StrafeHUD;
private:
	uint8 CharacterFireState;

	//CrossHair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrossHair", meta = (AllowPrivateAccess = "true"))
	float CrossHairSpreadBase;
	float CrossHairSpreadMultiplier;

	float CrossHairVelocityFactor;
	float CrossHairAimFactor;
	float CrossHairShootingFactor;
};
