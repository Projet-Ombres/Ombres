#include "LoadingScreen/Public/LoadingScreen.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "MoviePlayer.h"


IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreen);

DEFINE_LOG_CATEGORY(LoadingScreen)

#define LOCTEXT_NAMESPACE "LoadingScreen"

void FLoadingScreenModule::StartupModule()
{
	UE_LOG(LoadingScreen, Warning, TEXT("LoadingScreen: Log Started"));
}

void FLoadingScreenModule::ShutdownModule()
{
	UE_LOG(LoadingScreen, Warning, TEXT("LoadingScreen: Log Ended"));
}

void FLoadingScreenModule::StartInGameLoadingScreen(bool bPlayUntilStopped, float PlayTime)
{
	FLoadingScreenAttributes screen;
	screen.bAutoCompleteWhenLoadingCompletes = !bPlayUntilStopped;
	screen.bWaitForManualStop = bPlayUntilStopped;
	screen.bAllowEngineTick = bPlayUntilStopped;
	screen.MinimumLoadingScreenDisplayTime = 5;
	//screen.WidgetLoadingScreen = SNew(SStandardSlateWidget)
	GetMoviePlayer()->SetupLoadingScreen(screen);
	UE_LOG(LoadingScreen, Warning, TEXT("LOADING STARTED"));
}

void FLoadingScreenModule::StopInGameLoadingScreen() 
{
	GetMoviePlayer()->StopMovie();
	UE_LOG(LoadingScreen, Warning, TEXT("LOADING FINISHED"));
}


void FLoadingScreenModule::CreateScreen() {
	FLoadingScreenAttributes screen;
	screen.bAutoCompleteWhenLoadingCompletes = true;
	screen.MinimumLoadingScreenDisplayTime = 5;
	//screen.WidgetLoadingScreen = SNew(SStandardSlateWidget);
	
}


#undef LOCTEXT_NAMESPACE