// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "Engine/LevelStreaming.h"
#include "OmbresGameInstance.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBasicDelegate);

UCLASS()
class PROJECT_OMBRES_API UOmbresGameInstance : public UGameInstance
{
	GENERATED_BODY()



protected:

	virtual void Init() override;

	UFUNCTION()
		virtual void BeginLoadingScreen(const FString& MapName);

	UFUNCTION()
		virtual void EndLoadingScreen(UWorld* InLoadedWorld);

	

private:
	UPROPERTY()
		FTimerHandle timerHandle;

	UFUNCTION()
		void CheckIsLoadingSubLevels();

	UPROPERTY(BlueprintAssignable)
		FBasicDelegate OnFullLevelLoaded;

	UPROPERTY(BlueprintAssignable)
		FBasicDelegate OnBaseLevelLoaded;

	UPROPERTY()
		TArray<AActor*> StreamLevelActors;

	int streamLevelIndex;

	FTimerHandle streamLevelTimerHandle;

	UFUNCTION()
		void ShowNextActor();

	FStreamableManager Loader;

	UPROPERTY()
		ULevelStreaming* LevelToLoad;

	UPROPERTY()
		ULevel* LoadedLevel;

	UPROPERTY()
		FSoftObjectPath LevelSoftObjectPath;

public:
	UFUNCTION(BlueprintCallable)
	void LoadStreamLevelAsync(ULevelStreaming* Level);

	UFUNCTION()
	void OnLevelLoaded();
	
	
	
};
