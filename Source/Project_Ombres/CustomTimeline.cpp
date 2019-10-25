// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTimeline.h"
#include "Engine.h"
#include "Containers/UnrealString.h"



UCustomTimeline* UCustomTimeline::StartCustomTimeline(UObject* worldContextObject, float playRate,float& outputValue,float& realDeltaTime,UCustomTimeline*& ref)
{
	UCustomTimeline* newTimeline = NewObject<UCustomTimeline>();
	newTimeline->TickDelegate = FTickerDelegate::CreateUObject(newTimeline, &UCustomTimeline::Tick);
	newTimeline->TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(newTimeline->TickDelegate);
	newTimeline->WorldContextObject = worldContextObject;
	newTimeline->World = GEngine->GetWorldFromContextObjectChecked(worldContextObject);
	newTimeline->timerDuration = 1 / playRate;
	outputValue= 0;
	realDeltaTime = 0;
	newTimeline->realDeltaTime = &realDeltaTime;
	newTimeline->value = &outputValue;
	newTimeline->running = true;
	ref = newTimeline;
	return newTimeline;
}



void UCustomTimeline::StopCustomTimeline(UObject* worldContextObject, UCustomTimeline* ref)
{
	if (IsValid(ref)) {
		FTicker::GetCoreTicker().RemoveTicker(ref->TickDelegateHandle);
		ref->running = false;
		ref->Finished.Broadcast();
	}
	
}

bool UCustomTimeline::Tick(float DeltaTime)
{
	//UE_LOG(LogTemp, Log, TEXT("name : %s"),*GetName());
	if (!running) { return false; }

	*realDeltaTime = DeltaTime;

	*value += DeltaTime/timerDuration;

	if (*value < 1) {
		Update.Broadcast();
	}
	else {
		StopCustomTimeline(WorldContextObject, this);
	}
	return true;
}

