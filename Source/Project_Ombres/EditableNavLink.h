// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "Vector.h"
#include "NavigationSystem.h"
#include "Components/SphereComponent.h"
#include "EditableNavLink.generated.h"

/**
 * 
 */


USTRUCT()
struct FSphereCollision {
	GENERATED_BODY()
	int Index;
	bool Left;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;
};

USTRUCT()
struct FSphereCouple {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FSphereCollision leftSphere;
	UPROPERTY(VisibleAnywhere)
	FSphereCollision rightSphere;
};

UCLASS()
class PROJECT_OMBRES_API AEditableNavLink : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	AEditableNavLink();
	void SetSmartLinkPositions();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EditableNavLink, Meta = (MakeEditWidget = true))
	FVector left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EditableNavLink, Meta = (MakeEditWidget = true))
	FVector right;

#if WITH_EDITOR
	//je suis obligé de mettre l'implémentation ici pour qu'elle ne soit pas incluse dans les build (c'est une feature editor-only) et le WITH_EDITOR ne fonctionne que dans le header (RIP)
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FNavigationLink, Left) || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FNavigationLink, Right)) {
			SetSmartLinkPositions();
		}
	}

#endif

};
