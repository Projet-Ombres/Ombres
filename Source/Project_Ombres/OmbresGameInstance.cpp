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
		FString s = TEXT("Loading... ");

		
		return FText::FromString(s);
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
	LoadingScreenPhrases[0] = FText::FromString(TEXT("Diesel is the safest energy source in London!"));
	LoadingScreenPhrases[1] = FText::FromString(TEXT("The wardens can sometimes use more than 3L/h of diesel."));
	LoadingScreenPhrases[2] = FText::FromString(TEXT("A mecanical warden can reach a top speed of 40km/h. There is no point in resisting them!"));
	LoadingScreenPhrases[3] = FText::FromString(TEXT("The tallest building in our magnificient city is 312m high and is occupied by 250 people."));
	LoadingScreenPhrases[4] = FText::FromString(TEXT("London has more than 630km of pipe, delivering diesel or more and less clean air."));
	LoadingScreenPhrases[5] = FText::FromString(TEXT("The vertical tram station is more than 450m high! Be careful not to drop your ticket!"));
	LoadingScreenPhrases[6] = FText::FromString(TEXT("A tram ticket is only 6.30£, don't hesitate!"));
	LoadingScreenPhrases[7] = FText::FromString(TEXT("London alchemists are no longer welcome since one of them destroyed an entire factory!"));
	LoadingScreenPhrases[8] = FText::FromString(TEXT("Even if our diesel has a delicious green colour, don't drink it."));
	LoadingScreenPhrases[9] = FText::FromString(TEXT("Damage caused by the wardens are not refunded. Article 35 paragrash 8."));
	LoadingScreenPhrases[10] = FText::FromString(TEXT("A mecanical warden is faster, more efficient and more energetic than a human policeman."));
	LoadingScreenPhrases[11] = FText::FromString(TEXT("Many residents go to rooftops to breathe cleaner air as smoke spreads on the sidewalks."));
	LoadingScreenPhrases[12] = FText::FromString(TEXT("Don't hesitate to report any illegal alchemical behaviour. They are terrorists, and nobody likes terrorists."));
	LoadingScreenPhrases[13] = FText::FromString(TEXT("The wardens' voices are pre-recorded sounds, based on the voice of the governor \"Stan Jacob\"."));
	LoadingScreenPhrases[14] = FText::FromString(TEXT("London is the most evolved city in the world, thanks to you workers!"));
	LoadingScreenPhrases[15] = FText::FromString(TEXT("The tram was invented to go from district to district whithout passing through the congested roads."));
	LoadingScreenPhrases[16] = FText::FromString(TEXT("Thanks to our factories and the smoke they emit, London no longer has moskito problemes in summer. Another advantage of Diesel!"));
	LoadingScreenPhrases[17] = FText::FromString(TEXT("Alchemists are beings who have lost their humanity and practice black magic. Yuck!"));
	LoadingScreenPhrases[18] = FText::FromString(TEXT("Quarentine has been setup in order to purify the city of all traces of alchemy. Please help the government!"));
	LoadingScreenPhrases[19] = FText::FromString(TEXT("Diesel is the fuel of today and tomorrow!"));
	LoadingScreenPhrases[20] = FText::FromString(TEXT("The symbol consisting of a diagonal and vertical bar in circle, with a crown on the top is the anti-alchemical symbol of the government!"));
	LoadingScreenPhrases[21] = FText::FromString(TEXT("All the devices in London use Diesel from Diesel Corp!"));
	LoadingScreenPhrases[22] = FText::FromString(TEXT("Diesel Corp will hire new workers, right after the quarentine. Be patient!"));
	LoadingScreenPhrases[23] = FText::FromString(TEXT("Alchemy, compared to diesel, is a hazardous source of energy."));
	LoadingScreenPhrases[24] = FText::FromString(TEXT("If you hear screams during the quarentine, stay home. It means everything is going well!"));
	LoadingScreenPhrases[25] = FText::FromString(TEXT("The gears of the city partly mix Diesel in tanks and pipes."));
	LoadingScreenPhrases[26] = FText::FromString(TEXT("The street lamps on the roofs allow our engineers to work on the pipes, even at night!"));
	LoadingScreenPhrases[27] = FText::FromString(TEXT("The wardens have picks instead of hands to climb walls. Useful for the bad guys. "));


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
	UE_LOG(LogTemp, Warning, TEXT("Display loading screen"));
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreen.bMoviesAreSkippable = false;
	LoadingScreen.WidgetLoadingScreen = NewLoadingScreenWidget();
	LoadingScreen.bWaitForManualStop = false;
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UOmbresGameInstance::HideLoadingScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("Hide loading screen"));
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
