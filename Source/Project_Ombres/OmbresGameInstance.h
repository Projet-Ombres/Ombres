// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Widgets/SWidget.h"
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

public:
		static UOmbresGameInstance* Instance;

		UPROPERTY()
		FName LoadingLevelName;
		
		UFUNCTION(BlueprintCallable)
		float GetProgressPercent() const;

		UOmbresGameInstance(const FObjectInitializer& ObjectInitializer);

		UPROPERTY()
		TArray<FText> LoadingScreenPhrases;

		int randomPhraseIndex;

		UFUNCTION()
		FText GetCurrentPhrase() const;


protected:

	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& MapName);

	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

	UFUNCTION(BlueprintCallable)
		void DisplayLoadingScreen();

	UFUNCTION(BlueprintCallable)
		void HideLoadingScreen();

	UPROPERTY(BlueprintReadWrite)
	TArray<FName> StreamLevelsToLoad;

	

private:

	FTimerHandle randomPhrasesTimerHandle;

	UPROPERTY(BlueprintAssignable)
	FBasicDelegate OnFullLevelLoaded;

	UPROPERTY(BlueprintAssignable)
	FBasicDelegate OnBaseLevelLoaded;

	TSharedRef<SWidget> NewLoadingScreenWidget();

	UPROPERTY()
	TArray<class UTexture2D*> backgroundTextures;

	

	UFUNCTION()
		void ChangeRandomPhrase();
	
};

