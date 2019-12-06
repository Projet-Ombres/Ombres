// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelDesignGeneratedBase.generated.h"

UCLASS()
class PROJECT_OMBRES_API ALevelDesignGeneratedBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelDesignGeneratedBase();

	UPROPERTY()
	TArray<FVector> vertices;

	UPROPERTY()
	TArray<FVector> binormals;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* textRendersParent;

	UPROPERTY(EditAnywhere)
	FLinearColor textColor;

	UPROPERTY(EditInstanceOnly)
	bool bRebake;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override {
		Super::PostEditChangeProperty(PropertyChangedEvent);
		UpdateConstruction();
	}
#endif

public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;

private:
	void UpdateConstruction();

};
