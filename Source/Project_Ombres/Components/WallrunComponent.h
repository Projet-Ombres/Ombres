// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WallrunComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStopWallrunDelegate, float, WallRunTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWallrunDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_OMBRES_API UWallrunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallrunComponent();

	UPROPERTY(EditAnywhere,  meta = (ToolTip = "Duration of the application of the curve. The player is still attached to the wall after this time is elapsed but fall rapidly."))
		float Duration;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ToolTip = "Target roll angle."))
		float CameraRollAngle;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ToolTip = "Time the camera takes to change its roll (camera tint)."))
		float CameraRollTransitionDuration;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Maximum height reached by the player during a wallrun."))
		float CurveMaxHeight;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ToolTip = "Maximum angle to the wall to start wall running."))
		float MaxAngleToWall;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Time for the player to be snapped to the wall."))
		float SnapToWallDuration;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Maximum distance for the player to be snapped to the wall."))
		float SnapDistance;

	UPROPERTY(EditAnywhere, Category = "Jump", meta = (ToolTip = "Horizontal force applied during a wall jump."))
		float WallJumpForwardForce;

	UPROPERTY(EditAnywhere, Category = "Jump", meta = (ToolTip = "Force applied during a wall jump, in the direction of the wall normal."))
		float WallJumpNormalForce;

	UPROPERTY(EditAnywhere, Category = "Jump", meta=(ToolTip="Vertical force applied during a wall jump."))
		float WallJumpUpForce;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Curve that controls the height of the player. Values must be between [0-1]."))
		class UCurveFloat* HeightCurve;

	UPROPERTY(EditAnywhere, Category="Camera", meta = (ToolTip = "Curve that controls the roll angle of the camera. Values must be between [0-1]."))
		class UCurveFloat* CameraRollCurve;

	UPROPERTY(EditAnywhere,meta=(ToolTip="Curve controlling the speed of the player during a wall run."))
		class UCurveFloat* SpeedCurve;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "The player will stop wallrunning after this time without detecting walls."))
		float stopWallrunTolerance;

	UPROPERTY(EditAnywhere, Category="Camera", meta = (ToolTip = "The player will stop wallrunning after this time without detecting walls."))
		TSubclassOf<class UCameraShake> ScreenShake;



	UPROPERTY(BlueprintAssignable)
		FWallrunDelegate OnWallrunStart;

	UPROPERTY(BlueprintAssignable)
		FStopWallrunDelegate OnWallrunEnd;

	UPROPERTY(BlueprintAssignable)
		FWallrunDelegate OnWallJump;





	UPROPERTY(BlueprintReadOnly)
		FVector wallrunStartPoint;

	UPROPERTY(BlueprintReadWrite)
		bool bIsDoingSomethingElse;

	UPROPERTY(BlueprintReadWrite)
	bool bIsWallrunning;

	UPROPERTY(BlueprintReadOnly)
		float wallrunCurrentTime;

	UPROPERTY(BlueprintReadOnly)
		bool bInFrontOfWall;

	UPROPERTY(BlueprintReadOnly)
		AActor* lastRunnedWall;

	UPROPERTY(BlueprintReadOnly)
		FVector wallNormal;


private:
	bool bWallrunLeft;
	float minWallrunAngle;
	float maxWallrunAngle;
	FVector wallDirection;
	UPROPERTY()
	class ACharacter* playerCharacter;
	
	
	
	bool bRotationNeedsUpdate;
	bool bStoppingWallrun;
	
	
	float currentTimeStopWallrun;
	FRotator wallrunStartRotation;
	float distanceFromWall;
	FVector hitLocation;
	float currentCloseToWallTime;
	bool bJumpingOff;
	float wallrunLocalCurrentTime;
	float cameraRollStart;
	UPROPERTY()
		class UCameraShake* cameraShakeInstance;

	bool bNeedToStopCurve;
	bool bNeedToStopCameraUpdates;

	float cameraRollEndTime;

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
		void ResetOnLanded();

	UFUNCTION(BlueprintCallable)
		void StopWallrun();
	
	UFUNCTION(BlueprintCallable)
	void StopWallrunInstantly();

	UFUNCTION(BlueprintCallable)
		void JumpOffWall();


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UFUNCTION()
		void WallrunTracer();

	UFUNCTION()
		void SetCameraRoll(float Value);

	

	UFUNCTION()
	FVector CorrectDirectionVector(FVector InVector) const;

	UFUNCTION()
	float UnsignAngle(float SignedAngle) const;


	UFUNCTION()
	bool CompareAngles(float Angle1, float Angle2) const;



	UFUNCTION()
	float CalculateAngleToWall(AActor* WallActor, FVector ImpactPoint) const;

	UFUNCTION()
		void EndWallrun();

	


	UFUNCTION()
		void StartWallrun();

	UFUNCTION()
		void UpdateWallrunCurve(float Value);

	

	UFUNCTION()
		void UpdateCameraRoll(float Value);


	UFUNCTION()
		void StopCameraRoll();

	UFUNCTION()
		bool IsAllowedToWallrun();

	UFUNCTION()
		void DetachFromWall();
};
