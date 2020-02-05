// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadingScreenWidget.h"
#include "SlateOptMacros.h"
#include "Slate.h"
#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLoadingScreenWidget::Construct(const FArguments& InArgs)
{
	OwnerHUD = InArgs._OwnerHUDArg;


	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
			[
				SNew(SBorder)
				.BorderImage(new FSlateImageBrush("Texture2D'/Game/TemporaryAsset/Texture/MockupCity01.MockupCity01'", FVector2D(1920, 1080)))
				[
					SNew(STextBlock)
					.ShadowColorAndOpacity(FLinearColor::Black)
					.ColorAndOpacity(FLinearColor::Red)
					.ShadowOffset(FIntPoint(-1, 1))
					.Text(FText::FromString("Loading level..."))
				]
				
			]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
