// Fill out your copyright notice in the Description page of Project Settings.


#include "RecorderComponent.h"
#include "Paths.h"
#include "FileHelper.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


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

	playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}



void URecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	if (bPlayingBack) {
		frameTime += GetWorld()->GetDeltaSeconds();
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

void URecorderComponent::RecordCrystallizationPressed()
{
	if (bRecording) {
		FrameEvents += "CryP;";
	}
}

void URecorderComponent::RecordCrystallizationReleased() {
	if (bRecording) {
		FrameEvents += "CryR";
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

void URecorderComponent::StartNewRecording(int checkpointId,FString Level)
{
	if (!bPlayingBack) {
		UE_LOG(LogTemp, Warning, TEXT("STARTED NEW RECORDING"));
		GetRidOfOldFiles();
		SaveFileName = FPaths::MakeValidFileName(FDateTime::Now().ToString()) + ".txt";
		FString fileFullPath = FPaths::ProjectDir() + "/Records/" + SaveFileName;
		FFileHelper::SaveStringToFile("Lvl[" + Level + "](" + FString::FromInt(checkpointId) + ");Pos[" +playerCharacter->GetActorLocation().ToCompactString() + "];RotY["+FString::SanitizeFloat(playerController->GetControlRotation().Yaw)+"];RotP["+FString::SanitizeFloat(playerController->GetControlRotation().Pitch)+"];Vel["+playerCharacter->GetVelocity().ToCompactString()+"];" +LINE_TERMINATOR, *fileFullPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
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


	//setup player chara at the starting position, rotation and velocity
	FString firstLine = ContentToPlay[0];
	TArray<FString> firstLineEvents;
	firstLine.ParseIntoArray(firstLineEvents, TEXT(";"));
	FVector playerInitialPosition = GetVectorByTag(firstLineEvents, "Pos");
	float playerInitialYaw = GetFloatByTag(firstLineEvents, "RotY");
	float playerInitialPitch = GetFloatByTag(firstLineEvents, "RotP");
	FVector playerInitialVeloctity = GetVectorByTag(firstLineEvents, "Vel");

	bool s = playerCharacter->SetActorLocation(playerInitialPosition);
	UE_LOG(LogTemp, Warning, TEXT("initial location successful : %s"), s ? TEXT("true") : TEXT("false"));
	playerCharacter->GetCharacterMovement()->Velocity = playerInitialVeloctity;
	playerController->SetControlRotation(FRotator(playerInitialPitch, playerInitialYaw, 0));

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
		OnJumpRequest.Broadcast();
	}
	else if (event.StartsWith(TEXT("Sky"))) {
		OnSkywalkRequest.Broadcast();
	}
	else if (event.StartsWith(TEXT("CryP"))) {
		OnCrystallizationPressedRequest.Broadcast();
	}
	else if (event.StartsWith(TEXT("CryR"))) {
		OnCrystallizationReleasedRequest.Broadcast();
	}
	else if (event.StartsWith(TEXT("Sli"))) {
		OnSlideRequest.Broadcast();
	}
	else if (event.StartsWith(TEXT("Pin"))) {
		OnPingRequest.Broadcast();
	}
	else if (event.StartsWith(TEXT("180"))) {
		On180Request.Broadcast();
	}
	else if (event.StartsWith(TEXT("Pos"))) {
		FString inputValue;
		event.Split("[", new FString(), &inputValue);
		inputValue.Split("]", &inputValue, new FString());
		FVector position;
		position.InitFromString(inputValue);
		playerCharacter->SetActorLocation(position, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else if (event.StartsWith(TEXT("Yaw"))) {
		FString inputValue;
		event.Split("[", new FString(), &inputValue);
		inputValue.Split("]", &inputValue, new FString());
		playerController->SetControlRotation(FRotator(playerController->GetControlRotation().Pitch, FCString::Atof(*inputValue), playerController->GetControlRotation().Roll));
	}
	else if (event.StartsWith(TEXT("Pitch"))) {
		FString inputValue;
		event.Split("[", new FString(), &inputValue);
		inputValue.Split("]", &inputValue, new FString());
		playerController->SetControlRotation(FRotator(FCString::Atof(*inputValue), playerController->GetControlRotation().Yaw, playerController->GetControlRotation().Roll));
	}
}

void URecorderComponent::CheckProgress()
{
	if (ContentCurrentIndex < ContentToPlay.Num()) {
		float recordedTime;
		FString timeAsString;
		FString events;
		ContentToPlay[ContentCurrentIndex].Split(":", &timeAsString, &events);

		recordedTime = FCString::Atof(*timeAsString);
		if (recordedTime < frameTime) {
			PlayEvents(events);
			ContentCurrentIndex++;
			CheckProgress();
		}
	}
	else {
		//stop playback
		bPlayingBack = false;
	}
	
}

float URecorderComponent::GetCurrentRecordDuration()
{
	FString lastLine = ContentToPlay[ContentToPlay.Num() - 1];
	FString lastTime;
	lastLine.Split(":", &lastTime, nullptr);

	return FCString::Atof(*lastTime);
}


/**
 * Gets the value specified by the tag as a string. The correct syntax should be Tag[...]. Prefer the array version.
 *
 * @param events			The line where to look for
 * @param Tag				The tag to look for
 * @return					Returns the value found as string. If the tag is not found, returns an empty string
 **/
FString URecorderComponent::GetStringByTag(FString events, FString Tag)
{
	TArray<FString> eventsArray;
	events.ParseIntoArray(eventsArray, TEXT(";"));

	return GetStringByTag(eventsArray,Tag);
}

/**
 * Gets the value specified by the tag as a string. The correct syntax should be Tag[...]. 
 *
 * @param events			The line where to look for
 * @param Tag				The tag to look for
 * @return					Returns the value found as string. If the tag is not found, returns an empty string
 **/
FString URecorderComponent::GetStringByTag(TArray<FString> eventsArray, FString Tag)
{
	bool eventFound = false;
	FString eventToFind;

	for (int i = 0, l = eventsArray.Num(); i < l; i++) {
		FString currentEvent = eventsArray[i];
		UE_LOG(LogTemp, Warning, TEXT("current event : %s"), *currentEvent);
		if (currentEvent.Contains(Tag + "[")) {
			eventFound = true;
			eventToFind = currentEvent;
			break;
		}
		else {
			continue;
		}
	}
	FString value;
	if (eventFound) {
		eventToFind.Split(Tag + "[", nullptr, &eventToFind);

		eventToFind.Split("]", &value, nullptr);
	}
	
	return value;
}

/**
 * Gets the value specified by the tag as a vector. The correct syntax should be Tag[...]. Prefer the array version.
 *
 * @param events			The line where to look for
 * @param Tag				The tag to look for
 * @return					Returns the value found as vector. If the tag is not found, returns an zero vector.
 **/
FVector URecorderComponent::GetVectorByTag(FString events, FString Tag)
{
	FString value = GetStringByTag(events, Tag);
	FVector result;
	result.InitFromString(value);
	return result;
}

/**
 * Gets the value specified by the tag as a vector. The correct syntax should be Tag[...].
 *
 * @param events			The line where to look for
 * @param Tag				The tag to look for
 * @return					Returns the value found as vector. If the tag is not found, returns a zero vector.
 **/
FVector URecorderComponent::GetVectorByTag(TArray<FString> eventsArray, FString Tag)
{
	FString value = GetStringByTag(eventsArray, Tag);
	UE_LOG(LogTemp, Warning, TEXT("result as string : %s"), *value);
	FVector result;
	result.InitFromString(value);
	return result;
}

/**
 * Gets the value specified by the tag as a float. The correct syntax should be Tag[...]. Prefer the array version.
 *
 * @param events			The line where to look for
 * @param Tag				The tag to look for
 * @return					Returns the value found as float. If the tag is not found, returns zero.
 **/
float URecorderComponent::GetFloatByTag(FString events, FString Tag)
{
	FString value = GetStringByTag(events, Tag);
	float result = FCString::Atof(*value);
	return result;
}

/**
 * Gets the value specified by the tag as a float. The correct syntax should be Tag[...].
 *
 * @param events			The line where to look for
 * @param Tag				The tag to look for
 * @return					Returns the value found as float. If the tag is not found, returns zero.
 **/
float URecorderComponent::GetFloatByTag(TArray<FString> eventsArray, FString Tag)
{
	FString value = GetStringByTag(eventsArray, Tag);
	float result = FCString::Atof(*value);
	return result;
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

	FramesEvents.Add(FString::SanitizeFloat(frameTime)+":Pos[" + playerCharacter->GetActorLocation().ToCompactString() + "];Yaw["+FString::SanitizeFloat(playerController->GetControlRotation().Yaw)+"];Pitch["+FString::SanitizeFloat(playerController->GetControlRotation().Pitch)+"];");
	FFileHelper::SaveStringArrayToFile(FramesEvents, *fileFullPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	FramesEvents.Empty();

}
