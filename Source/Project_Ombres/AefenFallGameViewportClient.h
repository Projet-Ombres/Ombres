// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "AefenFallGameViewportClient.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECT_OMBRES_API UAefenFallGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
public:
	virtual void ReceivedFocus(FViewport* Viewport);

	UFUNCTION(BlueprintImplementableEvent)
		void OnFocusReceived();
	
};
