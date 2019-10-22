// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Curves/CurveFloat.h"
#include "Tickable.h"

#include "CustomTimeline.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutputPin);

/**
 * 
 */
UCLASS(BlueprintType)
class PROJECT_OMBRES_API UCustomTimeline : public UBlueprintAsyncActionBase,public FTickableGameObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOutputPin Update;
	UPROPERTY(BlueprintAssignable)
	FOutputPin Finished;
	float* value;
	int id;

	FTimerHandle timerHandle;

public:
	/** Node to use as a timeline, but anywhere. Starts a timeline, to stop it permaturely, use the StopCustomTimeline node and give it the ref of this timeline.
	*@param playRate - The rate at which the timeline will play. A rate of 1 takes 1 second to execute. A rate of 2 takes 0.5.
	@return OutputValue - The percentage of execution between 0 and 1 (alpha).
	@return Ref - The reference of this timeline.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start custom timeline", CompactNodeTitle="StartCustomTimeline",Keywords="Start Custom Timeline", HidePin = "worldContextObject", DefaultToSelf = "worldContextObject" ), Category = "Custom nodes")
	static UCustomTimeline* StartCustomTimeline(UObject* worldContextObject,float playRate, float& outputValue,UCustomTimeline*& ref);

	/**Node used to stop a timeline immediately. Triggers 'Finished' execution on the timeline.
	*@param ref - Reference to the timeline to stop.
	*/
	UFUNCTION(BlueprintCallable, meta=(HidePin="worldContextObject", DefaultToSelf = "worldContextObject"), Category = "Custom nodes")
	static void StopCustomTimeline(UObject* worldContextObject, UCustomTimeline* ref);

private:
	UObject* WorldContextObject;
	UWorld* World;
	float timerDuration;
	bool running;

protected:
	// Inherited via FTickableGameObject
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

};
