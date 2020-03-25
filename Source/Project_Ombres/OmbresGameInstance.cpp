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
	OnBaseLevelLoaded.Broadcast();
	GetWorld()->GetTimerManager().SetTimer(timerHandle,this, &UOmbresGameInstance::CheckIsLoadingSubLevels,1,true);
}


void UOmbresGameInstance::CheckIsLoadingSubLevels() {
	TArray<ULevelStreaming*> streamingLevels= GetWorld()->GetStreamingLevels();
	bool ready = true;
	for (int i = 0, l = streamingLevels.Num(); i < l; i++) {
		
		if (streamingLevels[i]->HasLoadRequestPending()) {
			ready = false;

			UE_LOG(LogTemp, Warning, TEXT("Level streaming %i loading"), i);
			break;
		}
	}
	

	if (ready) {
		GetMoviePlayer()->PassLoadingScreenWindowBackToGame();
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);

		OnFullLevelLoaded.Broadcast();
	}
}

void UOmbresGameInstance::ShowNextActor()
{
	if (streamLevelIndex < StreamLevelActors.Num()) {
		StreamLevelActors[streamLevelIndex]->bHidden = false;
		streamLevelIndex++;
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(streamLevelTimerHandle);
	}
	
}

void UOmbresGameInstance::LoadStreamLevelAsync(ULevelStreaming* Level)
{
	LevelToLoad = Level;

	UE_LOG(LogTemp, Warning, TEXT("level path name : %s"), *LevelToLoad->PackageNameToLoad.ToString());
	LevelSoftObjectPath = FSoftObjectPath(LevelToLoad->PackageNameToLoad.ToString());
	Loader.RequestAsyncLoad(LevelSoftObjectPath, FStreamableDelegate::CreateUObject(this, &UOmbresGameInstance::OnLevelLoaded));


	

}

void UOmbresGameInstance::OnLevelLoaded()
{
	if (LevelSoftObjectPath.ResolveObject()==nullptr) {
		UE_LOG(LogTemp, Error, TEXT("object null"));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("object not null"));
	}
	UE_LOG(LogTemp, Error, TEXT("uclass : %s"), *LevelSoftObjectPath.ResolveObject()->GetClass()->GetName());
	
	UE_LOG(LogTemp, Error, TEXT("contains map : %s"), Cast<UPackage>(LevelSoftObjectPath.ResolveObject())->ContainsMap()?TEXT("true"):TEXT("false"));
	LoadedLevel = Cast<ULevel>(LevelSoftObjectPath.ResolveObject());

	if (LoadedLevel != nullptr) {
		StreamLevelActors = LoadedLevel->Actors;
		streamLevelIndex = 0;

		GetWorld()->GetTimerManager().SetTimer(streamLevelTimerHandle, this, &UOmbresGameInstance::ShowNextActor, 0.1f, true);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Level null"));
	}
	
	
}


