// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AefenBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_OMBRES_API UAefenBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static class UHierarchicalInstancedStaticMeshComponent* AddHierarchicalInstancedStaticMeshComponent(AActor* target);
	

};
