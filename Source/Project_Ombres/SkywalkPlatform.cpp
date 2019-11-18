// Fill out your copyright notice in the Description page of Project Settings.


#include "SkywalkPlatform.h"
#include "TimerManager.h"

// Sets default values
ASkywalkPlatform::ASkywalkPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkywalkPlatform::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(timerHandle, this, &ASkywalkPlatform::DestroyActor, 1 / FadeTime);
	
}

// Called every frame
void ASkywalkPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkywalkPlatform::DestroyActor()
{
	Destroy();
}

