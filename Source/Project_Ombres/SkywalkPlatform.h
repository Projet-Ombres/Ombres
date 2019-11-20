// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkywalkPlatform.generated.h"

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FTimerHandle timerHandle;
	FTimerHandle secondLineTimerHandle;

	UFUNCTION()
	void DestroyActor();

	UFUNCTION()
	void SpawnSecondLine();
};
