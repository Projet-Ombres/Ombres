// Fill out your copyright notice in the Description page of Project Settings.


#include "AefenFallGameViewportClient.h"

void UAefenFallGameViewportClient::ReceivedFocus(FViewport* Viewport) {
	Super::ReceivedFocus(Viewport);
	OnFocusReceived();
}