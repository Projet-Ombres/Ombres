// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomFunctionLibrary.h"
#include "SlateBasics.h"
#include "Steamworks/Steamv142/sdk/public/steam/steam_api.h"
#include "Steamworks/Steamv142/sdk/public/steam/isteamuserstats.h"
#include "Steamworks/Steamv142/sdk/public/steam/isteamgameserverstats.h"


void UCustomFunctionLibrary::ResetSteamAchievement(FString achievementID) {
	// Initialiser Steam
	bool bRet = SteamAPI_Init();
	// Cr�er l'objet SteamAchievements si Steam a �t� initialis� avec succ�s. 
	if (bRet)
	{
		std::string achievementIDAsStdString(TCHAR_TO_UTF8(*achievementID));
		ISteamGameServerStats* serverStats = SteamGameServerStats();
		if (serverStats != NULL) {
			serverStats->ClearUserAchievement(SteamUser()->GetSteamID(), achievementIDAsStdString.c_str());
		}
		
	}
}



void UCustomFunctionLibrary::SetCollisionResponseToChannels(UPrimitiveComponent* Component, FCollisionResponseContainer collision) {
	Component->SetCollisionResponseToChannels(collision);
}



bool UCustomFunctionLibrary::CollisionEquals(FCollisionResponseContainer collision1, FCollisionResponseContainer collision2) {
	return collision1 == collision2;
}

FCollisionResponseContainer UCustomFunctionLibrary::GetCollisionResponseToChannels(UPrimitiveComponent* Component){
	return Component->GetCollisionResponseToChannels();
}



bool UCustomFunctionLibrary::IsGamePadConnected()
{
	auto genericApplication = FSlateApplication::Get().GetPlatformApplication();
	if (genericApplication.Get() != nullptr && genericApplication->IsGamepadAttached())
	{
		return true;
	}
	return false;
}


TArray<FString> UCustomFunctionLibrary::GetRecordContent(FString FileName) {
	FString directoryPath = FPaths::ProjectDir() + "Records";
	FJsonSerializableArray content;
	FString fileFullName = FPaths::ProjectDir() + "/Records/" + FileName;
	FFileHelper::LoadFileToStringArray(content, *fileFullName);
	return content;
}

TArray<FRecordInfo> UCustomFunctionLibrary::GetRecordsInfo()
{
	FString directoryPath = FPaths::ProjectDir() + "Records";
	TArray<FString> fileNames;
	if (FPaths::DirectoryExists(directoryPath)) {
		IFileManager::Get().FindFiles(fileNames, *directoryPath);
	};

	TArray<FRecordInfo> recordsInfo;

	for (int i = 0, l = fileNames.Num(); i < l; i++) {
		FJsonSerializableArray content;
		FString fileFullName = FPaths::ProjectDir() + "/Records/" + fileNames[i];
		FFileHelper::LoadFileToStringArray(content, *fileFullName);
		FString firstLine = content[0];
		FString levelName;
		firstLine.Split(TEXT("Lvl["), new FString(), &levelName);
		FString checkpointId;
		levelName.Split(TEXT("]("), &levelName, &checkpointId);
		checkpointId.Split(TEXT(")"), &checkpointId, new FString());
		

		FDateTime fileDate = IFileManager::Get().GetTimeStamp(*fileFullName);

		FRecordInfo recordInfo;
		recordInfo.Date = fileDate;
		recordInfo.LevelName = levelName;
		recordInfo.FileName = fileNames[i];
		recordInfo.CheckpointId = FCString::Atoi(*checkpointId);

		recordsInfo.Add(recordInfo);
	}
	
	return recordsInfo;
}
