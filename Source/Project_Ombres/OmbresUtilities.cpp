// Fill out your copyright notice in the Description page of Project Settings.


#include "OmbresUtilities.h"
#include "UObject/Package.h"

const FString UOmbresUtilities::EnumToString(const TCHAR* Enum, int32 EnumValue)
{
	
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
	if (!EnumPtr) {
		return TEXT("INVALID");
	}
		

#if WITH_EDITOR
	return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
#else
	return EnumPtr->GetEnumName(EnumValue);
#endif	
}
