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
							.Font(FSlateFontInfo(FPaths::ProjectContentDir() / TEXT("FONT/Frontage-Regular.otf"), 10))
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
								.Font(FSlateFontInfo(FPaths::ProjectContentDir() / TEXT("FONT/Frontage-3D.otf"), 16))
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

	LoadingScreenPhrases.SetNum(11, false);
	LoadingScreenPhrases[0] = FText::FromString(TEXT("Le diesel est la source d'energie la plus sure de Londres !"));
	LoadingScreenPhrases[1] = FText::FromString(TEXT("Les Gardiens peuvent parfois utiliser plus de 3L de diesel par heure."));
	LoadingScreenPhrases[2] = FText::FromString(TEXT("Un gardien mécanique peut aller à une vitesse de pointe de 40km / h ! Rien de sert de résister face à eux !"));
	LoadingScreenPhrases[3] = FText::FromString(TEXT("Le plus grand bâtiment de notre magnifique ville fait 312 mètres et est occupé par 250 personnes."));
	LoadingScreenPhrases[4] = FText::FromString(TEXT("Londres possède plus de 630 kilomètres de tuyaux, acheminant du diesel ou de l’air plus ou moins pur."));
	LoadingScreenPhrases[5] = FText::FromString(TEXT("La gare verticale fait plus de 450 mètres de haut ! Attention à ne pas faire tomber votre ticket !"));
	LoadingScreenPhrases[6] = FText::FromString(TEXT("Un ticket d'accès au tram n’est qu'à seulement 6.30£, n’hésitez pas !"));
	LoadingScreenPhrases[7] = FText::FromString(TEXT("Les alchimistes de Londres ne sont plus les bienvenues, depuis que l’un d’eux a détruit une usine entière !"));
	LoadingScreenPhrases[8] = FText::FromString(TEXT("Même si notre Diesel possède une délicieuse couleur verte, n’en buvez pas."));
	LoadingScreenPhrases[9] = FText::FromString(TEXT("Les dégâts causés par les gardiens ne sont pas remboursés. Article 35 alinéa 8."));
	LoadingScreenPhrases[10] = FText::FromString(TEXT("Un gardien mécanique est plus rapide, plus performant et plus énergique qu’un policier humain."));

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
