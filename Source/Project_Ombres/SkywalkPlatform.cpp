// Fill out your copyright notice in the Description page of Project Settings.


#include "SkywalkPlatform.h"
#include "TimerManager.h"
#include "ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SkywalkComponent.h"

// Sets default values
ASkywalkPlatform::ASkywalkPlatform()
{

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetVisibility(false);

	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetCollisionProfileName(FName(TEXT("BlockAllDynamic")));
	StaticMeshComponent->SetRelativeScale3D(FVector(3,3,3));
}



// Called when the game starts or when spawned
void ASkywalkPlatform::BeginPlay()
{
	Super::BeginPlay();
}

void ASkywalkPlatform::Init() {
	GetWorldTimerManager().SetTimer(timerHandle, this, &ASkywalkPlatform::DestroyActor, FadeTime);
}


void ASkywalkPlatform::DestroyActor()
{
	Destroy();
}


void ASkywalkPlatform::SetDelay(float value)
{
	GetWorld()->GetTimerManager().SetTimer(secondLineTimerHandle, this, &ASkywalkPlatform::SpawnSecondLine, value, false);
}


void ASkywalkPlatform::SpawnSecondLine() {
	skywalkComponent->SpawnSecondScrapsLine();
}
