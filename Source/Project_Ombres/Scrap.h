// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "SkywalkComponent.h"
#include "Scrap.generated.h"

UCLASS()
class PROJECT_OMBRES_API AScrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScrap();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY()
	UTimelineComponent* Timeline1;

	UPROPERTY()
	UTimelineComponent* Timeline2;

	UPROPERTY()
	UCurveFloat* FloatCurve1;

	UPROPERTY()
	UCurveFloat* FloatCurve2;

	UFUNCTION()
	void Timeline1UpdateCallback(float val);

	UFUNCTION()
	void Timeline2UpdateCallback(float val);

	UFUNCTION()
	void Timeline1FinishedCallback(float val);

	UFUNCTION()
	void Timeline2FinishedCallback(float val);



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartMovingToTarget(float NoiseAmplitude, ACharacter* Player, float ScrapBringDuration, float ScrapPlaceDuration, int LineIndex, int Line,float LifeTime);


private:
	ACharacter* Player;

	FVector StartPosition1;
	FVector StartPosition2;
	FVector NoiseRefPosition;
	FVector BigNoiseRefPosition;
	
	FVector CalculateScrapTargetPosition(FVector middlePosition, FVector rightOffset, int lineIndex);

	FVector CalculateScrapFinalPosition(FVector middlePosition, FVector rightOffset, int index);

	int LineIndex;
	int Line;
	float LifeTime;

	USkywalkComponent* Skywalk;

	FTimerHandle timerHandle;

	void ResetPhysics();

};
