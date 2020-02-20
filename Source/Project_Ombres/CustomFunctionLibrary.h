// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/StaticMesh.h"
#include "CustomFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_OMBRES_API UCustomFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable)
	static void GenerateLevelArchitectureLOD(UStaticMesh* staticMesh) {
		staticMesh->SetLODGroup(FName(TEXT("LevelArchitecture")));
	}

	UFUNCTION(BlueprintCallable)
	static UChildActorComponent* AddChildActor(AActor* target) {
		UChildActorComponent* actorComponent = NewObject<UChildActorComponent>(target, UChildActorComponent::StaticClass(), FName("ChildActorComponent"));
		actorComponent->SetChildActorClass(target->GetClass());
		actorComponent->CreateChildActor();
		actorComponent->SetupAttachment(target->GetRootComponent());
		actorComponent->RegisterComponent();
		return actorComponent;
	}
#endif



};
