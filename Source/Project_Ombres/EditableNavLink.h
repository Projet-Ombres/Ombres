// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "Vector.h"
#include "EditableNavLink.generated.h"

/**
 * 
 */

UCLASS()
class PROJECT_OMBRES_API AEditableNavLink : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	AEditableNavLink();
	void SetSmartLinkPositions();
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=EditableNavLink, Meta = (MakeEditWidget = true))
	FVector left;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = EditableNavLink, Meta = (MakeEditWidget = true))
	FVector right;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	

};
