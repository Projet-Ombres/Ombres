#pragma once

#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "LoadingScreen/Public/LoadingScreenInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LoadingScreen, All, All)

class FLoadingScreenModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual void StartInGameLoadingScreen(bool bPlayUntilStopped, float PlayTime);
	virtual void StopInGameLoadingScreen();
	virtual void CreateScreen();


	
};