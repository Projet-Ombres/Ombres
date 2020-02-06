// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenHUD.h"

#include "SLoadingScreenWidget.h"

// ++ The SWeakWidget we are using
#include "Widgets/SWeakWidget.h" 

// ++ To use GEngine. You can also import just "Engine.h" but some people mention this compiles slower than this header.
#include "Engine.h"


void ALoadingScreenHUD::BeginPlay()
{
	// So far only TSharedPtr<SMyUIWidget> has been created, now create the actual object.
	// Create a SMyUIWidget on heap, our MyUIWidget shared pointer provides handle to object
	// Widget will not self-destruct unless the HUD's SharedPtr (and all other SharedPtrs) are destroyed first.
	Widget = SNew(SLoadingScreenWidget).OwnerHUDArg(this);

	// Pass our viewport a weak ptr to our widget
	// Viewport's weak ptr will not give Viewport ownership of Widget
	GEngine->GameViewport->AddViewportWidgetContent(
		SNew(SWeakWidget)
		.PossiblyNullContent(Widget.ToSharedRef())
	);

	// Set widget's properties as visible (sets child widget's properties recursively)
	Widget->SetVisibility(EVisibility::Visible);

}