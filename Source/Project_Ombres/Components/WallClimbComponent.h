// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraShake.h"
#include "WallrunComponent.h"
#include "SkywalkComponent.h"
#include "Camera/CameraComponent.h"
#include "WallClimbComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEscalateDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_OMBRES_API UWallClimbComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallClimbComponent();

	UPROPERTY(BlueprintReadWrite)
	bool bIsEscalating;

	UPROPERTY(BlueprintReadOnly)
		FVector WallNormal;

	UPROPERTY(BlueprintReadOnly)
		bool bIsClimbingLedge;

	UPROPERTY(BlueprintReadOnly)
		FVector WallLocation;

	UPROPERTY(BlueprintReadOnly)
		FVector WallTopLocation;

	UPROPERTY(BlueprintReadOnly)
		float ClimbStartHeight;

	UPROPERTY()
		class ACharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly)
		bool bInFrontOfWall;

	UPROPERTY(EditAnywhere)
		float CameraAngleAllowed;

	UPROPERTY(BlueprintReadWrite)
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
		UCurveFloat* CameraUpCurve;

	UPROPERTY(EditAnywhere)
		float OneHandCorrectionOffset;

	UPROPERTY(BlueprintReadOnly)
	float LeftHandRotY;

	UPROPERTY(BlueprintReadOnly)
	float RightHandRotY;

	UPROPERTY(BlueprintReadOnly)
		float ClimbMontageStartTime;

	UPROPERTY(BlueprintAssignable)
		FEscalateDelegate OnEscalate;

	UPROPERTY(BlueprintAssignable)
		FEscalateDelegate OnLedgeClimb;
	
	UPROPERTY(BlueprintAssignable)
		FEscalateDelegate OnClimbStop;

	UPROPERTY(EditAnywhere)
		float CameraYawRotationDuration;

	UPROPERTY(EditAnywhere)
		float CameraPitchRotationDuration;

	UPROPERTY(BlueprintReadOnly)
	FVector LeftHandSpotToGrab;

	UPROPERTY(BlueprintReadOnly)
	FVector RightHandSpotToGrab;

	UPROPERTY(BlueprintReadOnly)
		float HeightToClimb;


private:
		float PlayerYaw;
		
		bool leftHandTracerTriggered;
		bool rightHandTracerTriggered;
		
		FVector LeftHandNormal;
		FVector RightHandNormal;
		
		UPROPERTY()
			AActor* LastWallJumped;
		bool bCanGrabLeft;
		bool bCanGrabRight;

		bool bCancellingClimb;

		bool applyingDecayingSpeed;

		float DecayingSpeedTimeLeft;
		
	

		FTimerHandle CancelClimbTimerHandle;
		float cancelCurrentTime;

		FVector LastNormal;
		float CameraStartPitch;
		float CameraTargetPitch;
		float CameraStartYaw;
		float CameraYawTarget;
		bool bWasReset;

		//for tracer
		FVector StartTracePoint;
		FVector EndTracePoint;

		FVector locationHit;

		bool canGrabLeft;
		bool canGrabRight;

		UPROPERTY()
			AActor* HitActor;

		UPROPERTY()
			APlayerCameraManager* CameraManager;

		UPROPERTY()
			USkywalkComponent* skywalkComponent;

		UPROPERTY()
			UWallrunComponent* wallrunComponent;

		UPROPERTY()
			APlayerController* playerController;

		float CameraYawRotationTimeLeft;
		bool bRotatingYaw;

		float CameraPitchRotationTimeLeft;
		bool bRotatingPitch;


		UPROPERTY()
			UCameraComponent* PlayerCamera;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void EscalateWall();

	UFUNCTION(BlueprintCallable)
		void ClimbLedge();

	UFUNCTION(BlueprintCallable)
		void StopClimbing();

	UFUNCTION(BlueprintCallable)
		void DetachFromWall();

	UFUNCTION(BlueprintCallable)
		void CancelClimb();

	UFUNCTION(BlueprintCallable)
		void StopCancellingClimb();

	UFUNCTION()
		void Drop();

	UFUNCTION(BlueprintCallable)
		void CameraUp(bool CameraUp, float ResetTimeDilation);

	UFUNCTION()
		void RotateCameraYaw();

	UFUNCTION(BlueprintCallable)
		bool ClampCamera(float Value);


private:
	void HeightTracer();
	
	void CorrectHandPositions(float LeftHandYRotation, float RightHandYRotation);
	void CalculateHandRotations();
	FVector CalculateClimbPosition();
	void CalculateTracePoints();
	FVector CalculateWallNormal();
	
};
