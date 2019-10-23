// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTimeline.h"
#include "Engine.h"





UCustomTimeline* UCustomTimeline::StartCustomTimeline(UObject* worldContextObject, float playRate,float& outputValue,UCustomTimeline*& ref)
{
	UCustomTimeline* newTimeline = NewObject<UCustomTimeline>();
	newTimeline->World = GEngine->GetWorldFromContextObjectChecked(worldContextObject);
	newTimeline->timerDuration = 1 / playRate;
	outputValue= 0;
	newTimeline->value = &outputValue;
	newTimeline->running = true;
	ref = newTimeline;
	return newTimeline;
}

void UCustomTimeline::StopCustomTimeline(UObject* worldContextObject, UCustomTimeline* ref)
{
	ref->running = false;
	ref->Finished.Broadcast();
	ref = nullptr;
}

void UCustomTimeline::Tick(float DeltaTime)
{
	if (!running) { return; }

	*value += DeltaTime/timerDuration;

	if (*value < 1) {
		Update.Broadcast();
	}
	else {
		running = false;
		Finished.Broadcast();
	}
}

TStatId UCustomTimeline::GetStatId() const
{
	return TStatId();
}
