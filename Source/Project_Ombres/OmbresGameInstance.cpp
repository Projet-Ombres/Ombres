// Fill out your copyright notice in the Description page of Project Settings.


#include "OmbresGameInstance.h"
#include "MoviePlayer.h"
#include "Engine.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SThrobber.h"
#include "Engine/World.h"

UOmbresGameInstance* UOmbresGameInstance::Instance;

class SLoadingScreenWidget : public SCompoundWidget
{
public:

	SLoadingScreenWidget() {
		
		backgroundImages.Add("/Game/TemporaryAsset/Texture/MockupCity01.MockupCity01");
		backgroundImages.Add("/Game/TemporaryAsset/Texture/MockupCity02.MockupCity02");
		backgroundImages.Add("/Game/TemporaryAsset/Texture/Mockup3Effects.Mockup3Effects");
		



	}



	SLATE_BEGIN_ARGS(SLoadingScreenWidget): _BackgroundImage1(nullptr),_BackgroundImage2(nullptr),_BackgroundImage3(nullptr) {}
	SLATE_ARGUMENT(UTexture2D*, BackgroundImage1);
	SLATE_ARGUMENT(UTexture2D*, BackgroundImage2);
	SLATE_ARGUMENT(UTexture2D*, BackgroundImage3);

	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs)
	{
		

		int randomIndex = FMath::RandRange(1, 3);
		UTexture2D* texture=nullptr;

		switch (randomIndex) {
		case 1:
			texture=InArgs._BackgroundImage1;
			break;
		case 2:
			texture = InArgs._BackgroundImage2;
			break;
		case 3:
			texture = InArgs._BackgroundImage3;
			break;
		}

		BackgroundImageBrush.SetResourceObject(texture);
		BackgroundImageBrush.SetImageSize(FVector2D(texture->GetSurfaceWidth(), texture->GetSurfaceHeight()));
		BackgroundImageBrush.ImageType = ESlateBrushImageType::FullColor;
		BackgroundImageBrush.DrawAs = ESlateBrushDrawType::Image;

		
		
		ChildSlot
			[
				SNew(SBorder)
				.BorderImage(&BackgroundImageBrush)
				.Content()
				[
					

					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Center)
					+ SVerticalBox::Slot()
					.VAlign(VAlign_Bottom)
					.HAlign(HAlign_Center)
					[
						SNew(SThrobber)
						.Visibility(this, &SLoadingScreenWidget::GetLoadIndicatorVisibility)
					]
					+ SVerticalBox::Slot()
					.VAlign(VAlign_Bottom)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Font(FSlateFontInfo(FName("/Game/UIAssets/timr45w_Font.timr45w_Font"),16))
						.Text(this, &SLoadingScreenWidget::GetProgressText)
						.Visibility(this, &SLoadingScreenWidget::GetMessageIndicatorVisibility)
					]
				]
			];
	}

protected:
	FText GetProgressText() const {
		float percent = 0;
		FString s = "";
		if (GetMoviePlayer()->IsLoadingFinished()) {
			percent = GetProgressPercent();
			s = TEXT("Loading streaming levels... ");
			s.Append(FString::SanitizeFloat(percent, 1));
			s.Append("%");
		}
		else {
			s = TEXT("Loading... ");
		}
		
		return FText::FromString(s);
	}

	float GetProgressPercent() const{
		TArray<ULevelStreaming*> streamingLevels = UOmbresGameInstance::Instance->GetWorld()->GetStreamingLevels();
		float percent;
		int streamingLevelsLoaded = 0;
		int streamingLevelsCount = streamingLevels.Num();
		for (int i = 0; i < streamingLevelsCount; i++) {
			if (streamingLevels[i]->IsLevelLoaded()) {
				streamingLevelsLoaded++;
			}
		}
		percent=(float)streamingLevelsLoaded / (float)streamingLevelsCount * 100;
		return percent;
	}


	FSlateBrush BackgroundImageBrush;

private:

	EVisibility GetLoadIndicatorVisibility() const
	{
		
		return GetMoviePlayer()->IsLoadingFinished() ? EVisibility::Collapsed : EVisibility::Visible;
	}

	EVisibility GetMessageIndicatorVisibility() const
	{
		return EVisibility::Visible;
	}

	TArray<FName> backgroundImages;
	
};


void UOmbresGameInstance::Init()
{
	Super::Init();
	Instance = this;
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UOmbresGameInstance::BeginLoadingScreen);
	
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UOmbresGameInstance::EndLoadingScreen);
}

void UOmbresGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	if (!IsRunningDedicatedServer())
	{
		LoadingLevelName = FName(*InMapName);
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.bMoviesAreSkippable = true;
		LoadingScreen.WidgetLoadingScreen = NewLoadingScreenWidget();
		LoadingScreen.bWaitForManualStop = false;
		

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);

	}
}

void UOmbresGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("Loading Complete"));
	for (int i = 0, l = StreamLevelsToLoad.Num(); i < l; i++) {
		FLatentActionInfo actionInfo;
		actionInfo.UUID = i;
		UGameplayStatics::LoadStreamLevel(GetWorld(), StreamLevelsToLoad[i], true, true, actionInfo);
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




TSharedRef<SWidget> UOmbresGameInstance::NewLoadingScreenWidget()
{
	return SNew(SLoadingScreenWidget).BackgroundImage1(backgroundTexture1).BackgroundImage2(backgroundTexture2).BackgroundImage3(backgroundTexture3);
}


UOmbresGameInstance::UOmbresGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {


	static ConstructorHelpers::FObjectFinder<UTexture2D> textureFinder1(TEXT("/Game/TemporaryAsset/Texture/MockupCity01.MockupCity01"));
	if (textureFinder1.Succeeded()) {
		backgroundTexture1 = textureFinder1.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> textureFinder2(TEXT("/Game/TemporaryAsset/Texture/MockupCity02.MockupCity02"));
	if (textureFinder2.Succeeded()) {
		backgroundTexture2 = textureFinder2.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> textureFinder3(TEXT("/Game/TemporaryAsset/Texture/Mockup3Effects.Mockup3Effects"));
	if (textureFinder3.Succeeded()) {
		backgroundTexture3 = textureFinder3.Object;
	}
}