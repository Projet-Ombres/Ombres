


#pragma once

#include "Engine/StreamableManager.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamer.generated.h"

UENUM()
enum Type
{
	/** Package failed to load */
	Failed,
	/** Package loaded successfully */
	Succeeded,
	/** Async loading was canceled */
	Canceled
};


UCLASS()
class PROJECT_OMBRES_API ALevelStreamer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelStreamer();

	UPROPERTY(EditAnywhere)
	class UBoxComponent* Box;

	UPROPERTY(EditAnywhere)
	FString LevelToLoad;

	FStreamableManager Loader;
	
	UPROPERTY()
	FSoftObjectPath level;

	UPROPERTY()
		ULevel* LoadedLevel;

	FTimerHandle timerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnLoadComplete();

	UFUNCTION()
		void UpdateWorld();
};


