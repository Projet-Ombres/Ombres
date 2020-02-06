// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadingScreenHUD.generated.h"

class SLoadingScreenWidget;

/**
 * 
 */
UCLASS()
class PROJECT_OMBRES_API ALoadingScreenHUD : public AHUD
{
	GENERATED_BODY()

public:
	TSharedPtr<SLoadingScreenWidget> Widget;

	void BeginPlay();
	
};
