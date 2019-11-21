// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "Scrap.generated.h"

UCLASS()
class PROJECT_OMBRES_API AScrap : public AActor
{
	GENERATED_BODY()
	
public:	
	AScrap();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;




protected:
	virtual void BeginPlay() override;


};
