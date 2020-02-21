// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
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

	
};
