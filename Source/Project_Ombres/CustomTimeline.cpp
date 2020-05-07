// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTimeline.h"
#include "Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Tickable.h"
#include "Containers/UnrealString.h"



UCustomTimeline::UCustomTimeline() {
	running = false;
}

UCustomTimeline* UCustomTimeline::StartCustomTimeline(UObject* worldContextObject, float playRate,float startTime,float& outputValue,float& realDeltaTime,UCustomTimeline*& ref)
{
	UCustomTimeline* newTimeline = NewObject<UCustomTimeline>();

	newTimeline->WorldContextObject = worldContextObject;
	newTimeline->World = GEngine->GetWorldFromContextObjectChecked(worldContextObject);
	newTimeline->timerDuration = 1 / playRate;
	outputValue= startTime;
	realDeltaTime = 0;
	newTimeline->realDeltaTime = &realDeltaTime;
	newTimeline->value = &outputValue;
	newTimeline->running = true;
	ref = newTimeline;
	return newTimeline;
}



void UCustomTimeline::FinishCustomTimeline(UObject* worldContextObject, UCustomTimeline* ref)
{

	//UE_LOG(LogTemp, Log, TEXT("name : %s"), *ref->GetName());


	if (IsValid(ref)) {
		ref->running = false;
		ref->Finished.Broadcast();
	}
	
}

void UCustomTimeline::StopCustomTimeline(UObject* worldContextObject, UCustomTimeline* ref)
{
	if (IsValid(ref)) {
		ref->running = false;
		
	}
}

void UCustomTimeline::Tick(float DeltaTime)
{
	if (!running) { return; }

	*realDeltaTime = DeltaTime;
	*value += DeltaTime/timerDuration;

	if (*value < 1) {
		Update.Broadcast();
	}
	else {
		FinishCustomTimeline(WorldContextObject, this);
	}
}

bool UCustomTimeline::IsTickable() const
{
	return running;
}

bool UCustomTimeline::IsTickableInEditor() const
{
	return false;
}

bool UCustomTimeline::IsTickableWhenPaused() const
{
	return false;
}



TStatId UCustomTimeline::GetStatId() const
{
	return TStatId();
}

UWorld* UCustomTimeline::GetWorld() const
{
	return nullptr;
}


bool UCustomTimeline::IsRunning() {
	return running;
}

