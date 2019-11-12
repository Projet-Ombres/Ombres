// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OmbresUtilities.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_OMBRES_API UOmbresUtilities : public UObject
{
	GENERATED_BODY()

public:
	static const FString EnumToString(const TCHAR* Enum, int32 EnumValue);
	
	
};
