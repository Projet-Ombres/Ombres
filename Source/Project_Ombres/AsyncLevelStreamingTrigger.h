// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AsyncLevelStreamingTrigger.generated.h"

UCLASS()
class PROJECT_OMBRES_API AAsyncLevelStreamingTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsyncLevelStreamingTrigger();

	UPROPERTY(EditAnywhere)
		FName MapName;

	UPROPERTY()
		FSoftObjectPath MapToLoad;

	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxComponent;

	UPROPERTY()
		TArray<AActor*> LevelActors;

	UPROPERTY()
		FTimerHandle TimerHandle;

	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void LoadLevelAsync();

	UFUNCTION()
		void OnAsyncLoadingComplete();

	UFUNCTION()
		void AddActor();
};
