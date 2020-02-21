// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncLevelStreamingTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "OmbresGameInstance.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "TimerManager.h"

// Sets default values
AAsyncLevelStreamingTrigger::AAsyncLevelStreamingTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAsyncLevelStreamingTrigger::OnOverlap);
	BoxComponent->SetBoxExtent(FVector(200, 200, 200));
	BoxComponent->RegisterComponent();
	RootComponent=BoxComponent;

}

void AAsyncLevelStreamingTrigger::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	LoadLevelAsync();
}

// Called when the game starts or when spawned
void AAsyncLevelStreamingTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAsyncLevelStreamingTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AAsyncLevelStreamingTrigger::LoadLevelAsync()
{
	MapToLoad = FSoftObjectPath(MapName.ToString());

	UE_LOG(LogTemp, Warning, TEXT("Loading level %s"), *MapToLoad.ToString());
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();




	StreamableManager.RequestAsyncLoad(MapToLoad, FStreamableDelegate::CreateUObject(this,&AAsyncLevelStreamingTrigger::OnAsyncLoadingComplete));

}

void AAsyncLevelStreamingTrigger::OnAsyncLoadingComplete()
{
	
	UE_LOG(LogTemp, Warning, TEXT("Level %s loaded"), *MapToLoad.GetAssetName());
	ULevel* Level = Cast<ULevel>(MapToLoad.ResolveObject());
	
	
	if (Level != nullptr) {
		LevelActors = Level->Actors;
		UE_LOG(LogTemp, Warning, TEXT("actors count : %d"), LevelActors.Num());

		GetWorldTimerManager().SetTimer(TimerHandle, this, &AAsyncLevelStreamingTrigger::AddActor,0.1f,true);
	}
	
}

void AAsyncLevelStreamingTrigger::AddActor()
{
	if (LevelActors.Num() > 0) {
		AActor* actor=GetWorld()->SpawnActor(AActor::StaticClass());
		actor = LevelActors[0];
		LevelActors.RemoveAt(0);
	}
}
