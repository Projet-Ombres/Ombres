// Fill out your copyright notice in the Description page of Project Settings.


#include "WallClimbComponent.h"

// Sets default values for this component's properties
UWallClimbComponent::UWallClimbComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CameraAngleAllowed = 80;
	SideTracersOffset = 30;
	EscalateImpulse = 950;
	DecayingSpeedDuration = 0.75f;
	DecayingMaxSpeed = 600;
	CancelTime = 0.2f;
}


// Called when the game starts
void UWallClimbComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWallClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

