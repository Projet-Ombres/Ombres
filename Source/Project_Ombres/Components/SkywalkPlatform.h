// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "ModuleManager.h"
#include "SkywalkPlatform.generated.h"

#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetEnumName((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )



UCLASS()
class PROJECT_OMBRES_API ASkywalkPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkywalkPlatform();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY()
	float FadeTime;

	UPROPERTY()
	class USkywalkComponent* skywalkComponent;

	UFUNCTION()
	void SetDelay(float value);

	UFUNCTION()
	void Init();

	UPROPERTY()
	TArray<FVector> firstLineNoiseRefPositions;

	UPROPERTY()
	TArray<FVector> firstLineBigNoiseRefPositions;

	UPROPERTY()
	TArray<FVector> secondLineNoiseRefPositions;

	UPROPERTY()
	TArray<FVector> secondLineBigNoiseRefPositions;

	UPROPERTY()
	float progression1;

	UPROPERTY()
	float progression2;


	UFUNCTION()
	void SpawnFirstLine();

	UFUNCTION()
	void MoveClosestScrap(int LineIndex, int Line);

	UPROPERTY()
	TArray<AActor*> activeScraps1;

	UPROPERTY()
	TArray<AActor*> activeScraps2;

	UPROPERTY()
	TArray<AActor*> levitatingScraps1;

	UPROPERTY()
	TArray<AActor*> levitatingScraps2;

	UPROPERTY()
	TArray<FVector> startPositions1;

	UPROPERTY()
	TArray<FVector> startPositions2;

	UPROPERTY()
	TArray<FVector> intermediatePositions1;

	UPROPERTY()
	TArray<FVector> intermediatePositions2;

	UPROPERTY()
	TArray<FRotator> startRotations1;

	UPROPERTY()
	TArray<FRotator> startRotations2;

	UPROPERTY()
		TArray<AActor*> previews1;

	UPROPERTY()
		TArray<AActor*> previews2;

	UFUNCTION()
	void AddScrap(AActor* scrapToAdd, int LineIndex, int Line);

	UPROPERTY()
	float NoiseAmplitude;
	UPROPERTY()
	float ScrapLifeTime;
	UPROPERTY()
	float BringScrapDuration;
	UPROPERTY()
	float PlaceScrapDuration;
	UPROPERTY()
	float SpawnDistance;

	UPROPERTY()
	UCurveFloat* FloatCurve1;

	UPROPERTY()
	UCurveFloat* FloatCurve2;

	UPROPERTY()
	UCurveFloat* RotationCurve;

	UPROPERTY()
	TArray<UStaticMesh*> tiles;

	UPROPERTY()
	TArray<UStaticMesh*> signs;

	UFUNCTION()
		void CalculateCommonPositions();

	UPROPERTY()
		FVector ScrapFinalMiddlePosition2;

	UPROPERTY()
		FVector ScrapFinalMiddlePosition;

	UPROPERTY()
		FVector ScrapMiddlePosition;

	UPROPERTY()
		FVector ScrapMiddlePosition2;

	UPROPERTY()
		FVector ScrapRightOffset;

	UFUNCTION()
		void SetPaused(bool paused);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ResetPhysics1();
	void ResetPhysics2();

	FVector CalculateScrapTargetPosition(int scrapIndex,int line);

	FVector CalculateScrapFinalPosition(int scrapIndex,int line);


private:

	FTimerHandle timerHandle;
	FTimerHandle timerHandle2;
	FTimerHandle firstLineTimerHandle;
	FTimerHandle secondLineTimerHandle;

	UFUNCTION()
	void DestroyActor();

	

	UFUNCTION()
	void SpawnSecondLine();

	UPROPERTY()
		bool EnablePreviews;

};
