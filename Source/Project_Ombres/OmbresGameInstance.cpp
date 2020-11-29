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

	SLATE_BEGIN_ARGS(SLoadingScreenWidget): _BackgroundImages(TArray<UTexture2D*>()) {}
	SLATE_ARGUMENT(TArray<UTexture2D*>, BackgroundImages);
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs){

		int randomIndex = FMath::RandRange(0, InArgs._BackgroundImages.Num()-1);
		UTexture2D* texture = InArgs._BackgroundImages[randomIndex];
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
					.VAlign(VAlign_Bottom)
					.HAlign(HAlign_Center)
					
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Center)
						+ SVerticalBox::Slot()
						.VAlign(VAlign_Bottom)
						.HAlign(HAlign_Center)

						[
							SNew(STextBlock)
							.Font(FSlateFontInfo(FPaths::GameContentDir() / TEXT("Slate/Frontage-Regular.otf"), 10))
							.Text(this, &SLoadingScreenWidget::GetRandomPhrase)
							.Margin(FMargin(0,0,0,30))
						]

						+ SVerticalBox::Slot()
						.VAlign(VAlign_Bottom)
						.HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							[
								SNew(STextBlock)
								.Font(FSlateFontInfo(FPaths::GameContentDir() / TEXT("Slate/Frontage-3D.otf"), 16))
								.Text(this, &SLoadingScreenWidget::GetProgressText)
								.Visibility(this, &SLoadingScreenWidget::GetMessageIndicatorVisibility)
								.Margin(FMargin(0,0,100,50))
							]
							+ SHorizontalBox::Slot()
							[
								SNew(SThrobber)
								.Visibility(this, &SLoadingScreenWidget::GetLoadIndicatorVisibility)
							]
							
						]
							
					]
					
				]
			];
	}

protected:
	FText GetProgressText() const {
		return UOmbresGameInstance::Instance->LoadingText;
	}

	

	FText GetRandomPhrase() const {
		return UOmbresGameInstance::Instance->LoadingScreenPhrases[UOmbresGameInstance::Instance->randomPhraseIndex];
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

	
};

float UOmbresGameInstance::GetProgressPercent() const {

	float percent;
	
	int streamingLevelsLoaded = 0;
	int streamingLevelsCount = StreamLevelsToLoad.Num();
	for (int i = 0; i < streamingLevelsCount; i++) {
		if (UGameplayStatics::GetStreamingLevel(GetWorld(), StreamLevelsToLoad[i])->HasLoadedLevel()) {
			streamingLevelsLoaded++;
		}
	}
	if (streamingLevelsCount > 0) {
		percent = (float)streamingLevelsLoaded / (float)streamingLevelsCount;
	}
	else {
		percent = 0;
	}
	return percent;
}



UOmbresGameInstance::UOmbresGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {


	static ConstructorHelpers::FObjectFinder<UTexture2D> textureFinder1(TEXT("/Game/TemporaryAsset/Texture/MockupCity01.MockupCity01"));
	if (textureFinder1.Succeeded()) {
		backgroundTextures.Add(textureFinder1.Object);
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> textureFinder2(TEXT("/Game/TemporaryAsset/Texture/MockupCity02.MockupCity02"));
	if (textureFinder2.Succeeded()) {
		backgroundTextures.Add(textureFinder2.Object);
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> textureFinder3(TEXT("/Game/TemporaryAsset/Texture/Mockup3Effects.Mockup3Effects"));
	if (textureFinder3.Succeeded()) {
		backgroundTextures.Add(textureFinder3.Object);
	}
	
	LoadingScreenPhrases.SetNum(28, false);
	LoadingScreenPhrases[0] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable","LS_00");
	LoadingScreenPhrases[1] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_01");
	LoadingScreenPhrases[2] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_02");
	LoadingScreenPhrases[3] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_03");
	LoadingScreenPhrases[4] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_04");
	LoadingScreenPhrases[5] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_05");
	LoadingScreenPhrases[6] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_06");
	LoadingScreenPhrases[7] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_07");
	LoadingScreenPhrases[8] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_08");
	LoadingScreenPhrases[9] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_09");
	LoadingScreenPhrases[10] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_10");
	LoadingScreenPhrases[11] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_11");
	LoadingScreenPhrases[12] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_12");
	LoadingScreenPhrases[13] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_13");
	LoadingScreenPhrases[14] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_14");
	LoadingScreenPhrases[15] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_15");
	LoadingScreenPhrases[16] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_16");
	LoadingScreenPhrases[17] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_17");
	LoadingScreenPhrases[18] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_18");
	LoadingScreenPhrases[19] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_19");
	LoadingScreenPhrases[20] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_20");
	LoadingScreenPhrases[21] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_21");
	LoadingScreenPhrases[22] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_22");
	LoadingScreenPhrases[23] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_23");
	LoadingScreenPhrases[24] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_24");
	LoadingScreenPhrases[25] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_25");
	LoadingScreenPhrases[26] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_26");
	LoadingScreenPhrases[27] = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "LS_27");
	
	LoadingText = FText::FromStringTable("/Game/BP/Localization/AlchemicalTitlesStringTable.AlchemicalTitlesStringTable", "UI_Loading");
}


FText UOmbresGameInstance::GetCurrentPhrase() const
{
	return LoadingScreenPhrases[randomPhraseIndex];
}

bool UOmbresGameInstance::IsAltTabbed() const
{
	
	ULocalPlayer* localPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer();
	if (localPlayer == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("local player null"));
		return false;
	}

	return !localPlayer->ViewportClient->Viewport || !localPlayer->ViewportClient->Viewport->IsForegroundWindow();
}

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
		
		DisplayLoadingScreen();
		//on change la phrase toutes les 5 secondes
		GetTimerManager().SetTimer(randomPhrasesTimerHandle, this, &UOmbresGameInstance::ChangeRandomPhrase, 5, true);

		

	}
}

TSharedRef<SWidget> UOmbresGameInstance::NewLoadingScreenWidget()
{
	return SNew(SLoadingScreenWidget).BackgroundImages(backgroundTextures);
}

void UOmbresGameInstance::ChangeRandomPhrase()
{
	randomPhraseIndex = FMath::RandRange(0, LoadingScreenPhrases.Num() - 1);
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

	HideLoadingScreen();

}

void UOmbresGameInstance::DisplayLoadingScreen()
{
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreen.bMoviesAreSkippable = false;
	LoadingScreen.WidgetLoadingScreen = NewLoadingScreenWidget();
	LoadingScreen.bWaitForManualStop = false;
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UOmbresGameInstance::HideLoadingScreen()
{
	GetMoviePlayer()->PassLoadingScreenWindowBackToGame();
}



bool UOmbresGameInstance::CheckIsLoadingSublevels()
{
	TArray<ULevelStreaming*> StreamingLevels = GetWorld()->GetStreamingLevels();
	bool stillLoading = false;
	for (int i = 0, l = StreamingLevels.Num(); i < l; i++) {
		if (StreamingLevels[i]->ShouldBeLoaded()  && (!StreamingLevels[i]->HasLoadedLevel() || StreamingLevels[i]->HasLoadRequestPending())) {
			stillLoading = true;
			break;
		}
	}
	return stillLoading;
}
