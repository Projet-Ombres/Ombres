// Fill out your copyright notice in the Description page of Project Settings.


#include "RecorderComponent.h"
#include "Paths.h"
#include "FileHelper.h"
#include "Engine/World.h"
#include "TimerManager.h"



URecorderComponent::URecorderComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	bRecording = false;
	SetComponentTickEnabled(false);
	SaveFileName = "PlayerRun.txt";
	WriteToFileInterval = 1;
}



void URecorderComponent::BeginPlay()
{
	Super::BeginPlay();

	

	

}



void URecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	frameTime += DeltaTime;

	if (!FrameEvents.IsEmpty()) {
		FString finalString = FString::SanitizeFloat(frameTime) + ":" + FrameEvents;
		FramesEvents.Add(finalString);
		FrameEvents.Empty();
	}
	
}

void URecorderComponent::RecordMovementInputForward(float value)
{
	FrameEvents += "MovF[" + FString::SanitizeFloat(value) + "];";
}

void URecorderComponent::RecordMovementInputRight(float value)
{
	FrameEvents += "MovR[" + FString::SanitizeFloat(value) + "];";
}


void URecorderComponent::RecordControlRotationYaw(float YawValue)
{
	FrameEvents += "RotY[" + FString::SanitizeFloat(YawValue)+"];";
}

void URecorderComponent::RecordControlRotationPitch(float PitchValue)
{
	FrameEvents += "RotP[" + FString::SanitizeFloat(PitchValue) + "];";
}


void URecorderComponent::RecordJump()
{
	FrameEvents += "Jum;";
}

void URecorderComponent::RecordSlide()
{
	FrameEvents += "Sli;";
}

void URecorderComponent::RecordSkywalk()
{
	FrameEvents += "Sky;";
}

void URecorderComponent::RecordPing()
{
	FrameEvents += "Pin;";
}

void URecorderComponent::RecordCrystallization()
{
	FrameEvents += "Cry;";
}

void URecorderComponent::Record180()
{
	FrameEvents += "180;";
}


void URecorderComponent::RecordRobot(FVector robotPosition)
{
	FrameEvents += "Rob["+robotPosition.ToCompactString()+"];";
}

void URecorderComponent::GetRidOfOldFiles()
{
	FString directoryPath = FPaths::ProjectDir() + "Records";
	if (!FPaths::DirectoryExists(directoryPath)) {
		bool directoryCreated = IPlatformFile::GetPlatformPhysical().CreateDirectory(*directoryPath);
		if (!directoryCreated) {
			UE_LOG(LogTemp, Warning, TEXT("Couldn't create Records directory"));
			return;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Records directory successfully created"));
		}
	};
	TArray<FString> fileNames;
	IFileManager::Get().FindFiles(fileNames, *directoryPath);

	if (fileNames.Num() > 9) {
		
		int filesToDelete = fileNames.Num() - 9;
		int filesDeleted = 0;
		UE_LOG(LogTemp, Warning, TEXT("FILES TO DELETE %d"), filesToDelete);


		//deletes all the oldest files and only keep 9 files
		while (filesDeleted < filesToDelete) {
			FString oldestFile;
			int oldestFileIndex;
			FDateTime oldestDate=FDateTime::MaxValue();
			for (int i = 0, l = fileNames.Num(); i < l; i++) {
				FDateTime fileDate = IFileManager::Get().GetTimeStamp(*fileNames[i]);

				if (fileDate < oldestDate) {
					oldestDate = fileDate;
					oldestFile = fileNames[i];
					oldestFileIndex = i;
				}
			}
			oldestFile = FPaths::ConvertRelativePathToFull(directoryPath + "/" + oldestFile);
			bool fileDeleted = IFileManager::Get().Delete(*oldestFile,true);
			if (fileDeleted) {
				UE_LOG(LogTemp, Warning, TEXT("DELETED FILE %s"), *oldestFile);
			}
			fileNames.RemoveAt(oldestFileIndex);
			filesDeleted++;
		}
		
	}
	
}

void URecorderComponent::StartNewRecording()
{
	GetRidOfOldFiles();
	SaveFileName = FPaths::MakeValidFileName(FDateTime::Now().ToString())+".txt";
	UE_LOG(LogTemp, Warning, TEXT("save file name : %s"), *SaveFileName);
	ResumeRecording();
}


void URecorderComponent::PauseRecording()
{
	bRecording = false;
	SetComponentTickEnabled(false);
	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	WriteToFile();
}




void URecorderComponent::ResumeRecording()
{
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &URecorderComponent::WriteToFile, WriteToFileInterval, true);

	

	PrimaryComponentTick.bCanEverTick = true;
	bRecording = true;
	SetComponentTickEnabled(true);
}



void URecorderComponent::WriteToFile()
{
	FString fileFullPath = FPaths::ProjectDir() + "/Records/" + SaveFileName;
	FFileHelper::SaveStringArrayToFile(FramesEvents, *fileFullPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	FramesEvents.Empty();

}