// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "MeshInstancier.generated.h"

UCLASS()
class PROJECT_OMBRES_API AMeshInstancier : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshInstancier();

	UPROPERTY(VisibleAnywhere)
		class UHierarchicalInstancedStaticMeshComponent* HierarchicalInstancedStaticMeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
		UHierarchicalInstancedStaticMeshComponent* AddHierarchicalComponent();


};
