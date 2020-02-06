// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PROJECT_OMBRES_API SLoadingScreenWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreenWidget)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<class ALoadingScreenHUD>, OwnerHUDArg);
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<class ALoadingScreenHUD> OwnerHUD;
};
