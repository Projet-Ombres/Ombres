// Fill out your copyright notice in the Description page of Project Settings.


#include "RecorderComponent.h"
#include "Paths.h"
#include "FileHelper.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


URecorderComponent::URecorderComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	bRecording = false;
	bPlayingBack = false;
	SetComponentTickEnabled(false);
	WriteToFileInterval = 1;
}



void URecorderComponent::BeginPlay()
{
	Super::BeginPlay();
	bRecording = false;
	bPlayingBack = false;
	SetComponentTickEnabled(false);
}



void URecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	if (bPlayingBack) {
		frameTime += GetWorld()->GetDeltaSeconds();
		UE_LOG(LogTemp, Warning, TEXT("frame time : %f"), frameTime);
		UE_LOG(LogTemp, Warning, TEXT("DELTA TIME: %f"), GetWorld()->GetDeltaSeconds());
		CheckProgress();
	}
	else if (bRecording) {
		frameTime += DeltaTime;
		if (!FrameEvents.IsEmpty()) {
			FString finalString = FString::SanitizeFloat(frameTime) + ":" + FrameEvents;
			FramesEvents.Add(finalString);
			FrameEvents.Empty();
		}
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void URecorderComponent::RecordMovementInputForward(float value)
{
	if (bRecording) {
		FrameEvents += "MovF[" + FString::SanitizeFloat(value) + "];";
	}
}

void URecorderComponent::RecordMovementInputRight(float value)
{
	if (bRecording) {
		FrameEvents += "MovR[" + FString::SanitizeFloat(value) + "];";
	}
}


void URecorderComponent::RecordControlRotationYaw(float YawValue)
{
	if (bRecording) {
		FrameEvents += "RotY[" + FString::SanitizeFloat(YawValue) + "];";
	}
}

void URecorderComponent::RecordControlRotationPitch(float PitchValue)
{
	if (bRecording) {
		FrameEvents += "RotP[" + FString::SanitizeFloat(PitchValue) + "];";
	}
}


void URecorderComponent::RecordJump()
{
	if (bRecording) {
		FrameEvents += "Jum;";
	}
}

void URecorderComponent::RecordSlide()
{
	if (bRecording) {
		FrameEvents += "Sli;";
	}
}

void URecorderComponent::RecordSkywalk()
{
	if (bRecording) {
		FrameEvents += "Sky;";
	}
}

void URecorderComponent::RecordPing()
{
	if (bRecording) {
		FrameEvents += "Pin;";
	}
}

void URecorderComponent::RecordCrystallization()
{
	if (bRecording) {
		FrameEvents += "Cry;";
	}
}

void URecorderComponent::Record180()
{
	if (bRecording) {
		FrameEvents += "180;";
	}
}


void URecorderComponent::RecordRobot(FVector robotPosition)
{
	if (bRecording) {
		FrameEvents += "Rob[" + robotPosition.ToCompactString() + "];";
	}
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

void URecorderComponent::StartNewRecording(int checkpointId)
{
	if (!bPlayingBack) {
		UE_LOG(LogTemp, Warning, TEXT("STARTED NEW RECORDING"));
		GetRidOfOldFiles();
		SaveFileName = FPaths::MakeValidFileName(FDateTime::Now().ToString()) + ".txt";
		FString fileFullPath = FPaths::ProjectDir() + "/Records/" + SaveFileName;
		FFileHelper::SaveStringToFile("Lvl[" + GetWorld()->GetName() + "](" + FString::FromInt(checkpointId) + ");Pos[" + UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation().ToCompactString() + "]" LINE_TERMINATOR, *fileFullPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		recordingPaused = true;
		ResumeRecording();
	}
	
}

void URecorderComponent::PlayBack(TArray<FString> RecordContent)
{
	ContentCurrentIndex = 0;
	frameTime = 0;
	bPlayingBack = true;
	bRecording = false;
	ContentToPlay = RecordContent;
	SetComponentTickEnabled(true);
	UE_LOG(LogTemp, Warning, TEXT("Playback started"));
}

void URecorderComponent::PlayEvents(FString events)
{
	TArray<FString> separatedEvents;
	events.ParseIntoArray(separatedEvents, TEXT(";"));

	for (int i = 0, l = separatedEvents.Num(); i < l; i++) {
		PlayEvent(separatedEvents[i]);
	}
}

void URecorderComponent::PlayEvent(FString event) {
	if (event.StartsWith(TEXT("MovR"))) {
		FString inputValue;
		event.Split("[", new FString(), &inputValue);
		inputValue.Split("]", &inputValue, new FString());

		OnMoveRightRequest.Broadcast(FCString::Atof(*inputValue));
	}
	else if (event.StartsWith(TEXT("MovF"))) {
		FString inputValue;
		event.Split("[", new FString(), &inputValue);
		inputValue.Split("]", &inputValue, new FString());

		OnMoveForwardRequest.Broadcast(FCString::Atof(*inputValue));
	}
	else if (event.StartsWith(TEXT("RotY"))) {
		FString inputValue;
		event.Split("[", new FString(), &inputValue);
		inputValue.Split("]", &inputValue, new FString());

		OnRotationYawRequest.Broadcast(FCString::Atof(*inputValue));
	}
	else if (event.StartsWith(TEXT("RotP"))) {
		FString inputValue;
		event.Split("[", new FString(), &inputValue);
		inputValue.Split("]", &inputValue, new FString());

		OnRotationPitchRequest.Broadcast(FCString::Atof(*inputValue));
	}
	else if (event.StartsWith(TEXT("Jum"))) {
		//call jump
	}
}

void URecorderComponent::CheckProgress()
{
	float recordedTime;
	FString timeAsString;
	FString events;
	ContentToPlay[ContentCurrentIndex].Split(":", &timeAsString, &events);
	UE_LOG(LogTemp, Warning, TEXT("recorded time : %s"), *timeAsString);
	
	recordedTime = FCString::Atof(*timeAsString);
	if (recordedTime < frameTime) {
		PlayEvents(events);
		ContentCurrentIndex++;
		CheckProgress();
	}
}


void URecorderComponent::PauseRecording()
{
	if (bRecording) {
		recordingPaused = true;
		bRecording = false;
		SetComponentTickEnabled(false);
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		WriteToFile();
	}
	
}




void URecorderComponent::ResumeRecording()
{
	if (!bRecording && recordingPaused && !bPlayingBack) {
		UE_LOG(LogTemp, Warning, TEXT("RESUME RECORDING"));
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &URecorderComponent::WriteToFile, WriteToFileInterval, true);

		bRecording = true;
		recordingPaused = false;
		SetComponentTickEnabled(true);
	}
	
}



void URecorderComponent::WriteToFile()
{
	FString fileFullPath = FPaths::ProjectDir() + "/Records/" + SaveFileName;
	FramesEvents.Add("Pos[" + UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation().ToCompactString() + "];");
	FFileHelper::SaveStringArrayToFile(FramesEvents, *fileFullPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	FramesEvents.Empty();

}