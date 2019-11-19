// Fill out your copyright notice in the Description page of Project Settings.


#include "SkywalkPlatform.h"
#include "TimerManager.h"
#include "ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASkywalkPlatform::ASkywalkPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetVisibility(false);

	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
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

