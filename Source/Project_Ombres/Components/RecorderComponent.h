// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RecorderComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_OMBRES_API URecorderComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<FString> FramesEvents;

	FString FrameEvents;

	float frameTime;

	FTimerHandle timerHandle;

	UFUNCTION()
		void WriteToFile();


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
		void RecordCrystallization();

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

	UFUNCTION(BlueprintCallable)
		void StartNewRecording();




		
};
