// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraShake.h"
#include "WallClimbComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_OMBRES_API UWallClimbComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallClimbComponent();

	UPROPERTY(BlueprintReadOnly)
	bool bIsEscalating;

	UPROPERTY(BlueprintReadOnly)
		FVector WallNormal;

	UPROPERTY(BlueprintReadOnly)
		bool bIsClimbingLedge;

	UPROPERTY(BlueprintReadOnly)
		FVector WallLocation;

	UPROPERTY(BlueprintReadOnly)
		FVector WallTopLocation;

	UPROPERTY()
		float ClimbStartHeight;

	UPROPERTY()
		class ACharacter* PlayerCharacter;

	UPROPERTY()
		bool bInFrontOfWall;

	UPROPERTY(EditAnywhere)
		float CameraAngleAllowed;

	UPROPERTY(BlueprintReadOnly)
		bool bIsPropulsed;

	UPROPERTY(EditAnywhere)
		float SideTracersOffset;

	UPROPERTY(EditAnywhere)
	float HandPositionCorrectionMultiplier;

	UPROPERTY(EditAnywhere)
		float EscalateImpulse;

	UPROPERTY(EditAnywhere)
		float DecayingSpeedDuration;

	UPROPERTY(EditAnywhere)
		float DecayingMaxSpeed;

	UPROPERTY(EditAnywhere)
		float CancelTime;

	UPROPERTY(EditAnywhere)
		float CameraUpPitch;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CameraAnimation;

	UPROPERTY(EditAnywhere)
		float OneHandCorrectionOffset;
	
private:
		float PlayerYaw;
		
		bool leftHandTracerTriggered;
		bool rightHandTracerTriggered;
		FVector LeftHandSpotToGrab;
		FVector LeftHandNormal;
		FVector RightHandSpotToGrab;
		FVector RightHandNormal;
		
		UPROPERTY()
			AActor* LastWallJumped;
		bool bCanGrabLeft;
		bool bCanGrabRight;

		bool bCancellingClimb;
	

		FTimerHandle CancelClimbTimerHandle;

		FVector LastNormal;
		float CameraStartPitch;
		bool bWasReset;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
