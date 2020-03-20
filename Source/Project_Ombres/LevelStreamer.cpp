// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamer.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"


// Sets default values
ALevelStreamer::ALevelStreamer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	Box->RegisterComponent();
	Box->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamer::OnComponentOverlap);
}

// Called when the game starts or when spawned
void ALevelStreamer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelStreamer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelStreamer::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LoadPackageAsync(LevelToLoad.ToString(), FLoadPackageAsyncDelegate::CreateUObject(this, &ALevelStreamer::OnLoadComplete), 0, PKG_ContainsMap);
	level=FSoftObjectPath(LevelToLoad);
	Loader.RequestAsyncLoad(level, FStreamableDelegate::CreateUObject(this,&ALevelStreamer::OnLoadComplete));
}

void ALevelStreamer::OnLoadComplete()
{
	LoadedLevel=Cast<ULevel>(level.ResolveObject());
	check(LoadedLevel != nullptr);
	
	GetWorldTimerManager().SetTimer(timerHandle, this, &ALevelStreamer::UpdateWorld,0.5f,true);
	



}

void ALevelStreamer::UpdateWorld() {
	GetWorld()->AddToWorld(LoadedLevel);
	bool success = (LoadedLevel->OwningWorld == GetWorld());
	UE_LOG(LogTemp, Warning, TEXT("updated world : %s"), success ? TEXT("true") : TEXT("false"));
}

