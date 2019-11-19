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

	UPROPERTY(VisibleAnywhere)
	TArray<FSphereCouple> SphereCouples;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SphereComponentsParent;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPointReached(AActor* agent,FVector targetPosition);



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

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ResetReferences();
};
