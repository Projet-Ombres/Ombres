// Fill out your copyright notice in the Description page of Project Settings.


#include "OmbresGameInstance.h"
#include "MoviePlayer.h"
#include "Engine.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"

void UOmbresGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UOmbresGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UOmbresGameInstance::EndLoadingScreen);
}

void UOmbresGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.bMoviesAreSkippable = true;
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
		LoadingScreen.bWaitForManualStop = false;
		

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UOmbresGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("Loading Complete"));
	for (int i = 0, l = StreamLevelsToLoad.Num(); i < l; i++) {
		UGameplayStatics::LoadStreamLevel(GetWorld(), StreamLevelsToLoad[i], true, true, FLatentActionInfo());
	}
	OnBaseLevelLoaded.Broadcast();
	GetWorld()->GetTimerManager().SetTimer(timerHandle,this, &UOmbresGameInstance::CheckIsLoadingSubLevels,1,true,1);
}


void UOmbresGameInstance::CheckIsLoadingSubLevels() {
	TArray<ULevelStreaming*> streamingLevels= GetWorld()->GetStreamingLevels();
	bool ready = true;
	for (int i = 0, l = streamingLevels.Num(); i < l; i++) {
		
		if (streamingLevels[i]->HasLoadRequestPending() || streamingLevels[i]->IsStreamingStatePending()) {
			ready = false;

			UE_LOG(LogTemp, Warning, TEXT("Level streaming %s loading"), *streamingLevels[i]->GetName());
			break;
		}
	}
	

	if (ready) {
		GetMoviePlayer()->PassLoadingScreenWindowBackToGame();
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);

		OnFullLevelLoaded.Broadcast();
	}
}


