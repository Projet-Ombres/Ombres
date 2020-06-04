// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RecorderComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovementDelegate,float,InputValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecordDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_OMBRES_API URecorderComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<FString> FramesEvents;

	FString FrameEvents;

	UPROPERTY()
	float frameTime;

	FTimerHandle timerHandle;

	UFUNCTION()
		void WriteToFile();

	bool recordingPaused;

	UPROPERTY()
		ACharacter* playerCharacter;

	UPROPERTY()
		APlayerController* playerController;

	

public:	
	// Sets default values for this component's properties
	URecorderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void RecordMovementInputForward(float value);

	UFUNCTION(BlueprintCallable)
		void RecordMovementInputRight(float value);

	UFUNCTION(BlueprintCallable)
		void RecordControlRotationYaw(float YawValue);

	UFUNCTION(BlueprintCallable)
		void RecordControlRotationPitch(float PitchValue);

	UFUNCTION(BlueprintCallable)
		void RecordJump();

	UFUNCTION(BlueprintCallable)
		void RecordSlide();

	UFUNCTION(BlueprintCallable)
		void RecordSkywalk();

	UFUNCTION(BlueprintCallable)
		void RecordPing();

	UFUNCTION(BlueprintCallable)
		void RecordCrystallizationPressed();

	UFUNCTION(BlueprintCallable)
		void RecordCrystallizationReleased();

	UFUNCTION(BlueprintCallable)
		void Record180();


	UFUNCTION(BlueprintCallable)
		void PauseRecording();

	UFUNCTION(BlueprintCallable)
		void ResumeRecording();

	UFUNCTION(BlueprintCallable)
		void RecordRobot(FVector robotPosition);

	UFUNCTION(BlueprintCallable)
		void GetRidOfOldFiles();

	UPROPERTY()
		FString SaveFileName;

	UPROPERTY(EditAnywhere)
		float WriteToFileInterval;

	UPROPERTY(BlueprintReadOnly)
		bool bRecording;

	UPROPERTY(BlueprintReadOnly)
		bool bPlayingBack;

	UFUNCTION(BlueprintCallable)
		void StartNewRecording(int checkpointId);

	UFUNCTION(BlueprintCallable)
		void PlayBack(TArray<FString> RecordContent);

	UPROPERTY()
		TArray<FString> ContentToPlay;

	UPROPERTY()
		int ContentCurrentIndex;

	UFUNCTION()
		void PlayEvents(FString events);

	UFUNCTION()
		void PlayEvent(FString event);

	UFUNCTION()
		void CheckProgress();

	UFUNCTION(BlueprintCallable)
		float GetCurrentRecordDuration();


	


		FString GetStringByTag(FString events, FString Tag);

	UFUNCTION()
		FString GetStringByTag(TArray<FString> eventsArray, FString Tag);


		float GetFloatByTag(FString events, FString Tag);

	UFUNCTION()
		float GetFloatByTag(TArray<FString> eventsArray, FString Tag);


		FVector GetVectorByTag(FString events, FString Tag);

	UFUNCTION()
		FVector GetVectorByTag(TArray<FString> eventsArray, FString Tag);

	UPROPERTY(BlueprintAssignable)
		FMovementDelegate OnMoveForwardRequest;

	UPROPERTY(BlueprintAssignable)
		FMovementDelegate OnMoveRightRequest;

	UPROPERTY(BlueprintAssignable)
		FMovementDelegate OnRotationYawRequest;

	UPROPERTY(BlueprintAssignable)
		FMovementDelegate OnRotationPitchRequest;

	UPROPERTY(BlueprintAssignable)
		FRecordDelegate OnJumpRequest;

	UPROPERTY(BlueprintAssignable)
		FRecordDelegate OnPingRequest;

	UPROPERTY(BlueprintAssignable)
		FRecordDelegate OnSkywalkRequest;

	UPROPERTY(BlueprintAssignable)
		FRecordDelegate OnCrystallizationPressedRequest;

	UPROPERTY(BlueprintAssignable)
		FRecordDelegate OnCrystallizationReleasedRequest;

	UPROPERTY(BlueprintAssignable)
		FRecordDelegate On180Request;

	UPROPERTY(BlueprintAssignable)
		FRecordDelegate OnSlideRequest;
		
};
