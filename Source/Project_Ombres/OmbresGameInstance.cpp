// Fill out your copyright notice in the Description page of Project Settings.


#include "OmbresGameInstance.h"

void UOmbresGameInstance::LoadComplete(const float LoadTime, const FString& MapName) {
	UE_LOG(LogTemp, Warning, (TEXT("Level loaded : %s")), *MapName);
}