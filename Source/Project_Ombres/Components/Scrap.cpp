// Fill out your copyright notice in the Description page of Project Settings.


#include "Scrap.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "ConstructorHelpers.h"

// Sets default values
AScrap::AScrap()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"), false);
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"), false);
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);


	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("StaticMesh'/Engine/BasicShapes/BasicShapeMaterial'"));
	StaticMeshComponent->SetMaterial(0, Material.Object);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionProfileName(FName(TEXT("Custom")));
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);


	StaticMeshComponent->SetGenerateOverlapEvents(false);

	

	Tags.Add(FName(TEXT("Scrap")));
}


void AScrap::BeginPlay()
{
	Super::BeginPlay();
	//StaticMeshComponent->SetWorldScale3D(FVector(FMath::FRandRange(0.2,0.8), FMath::FRandRange(0.2, 0.8), FMath::FRandRange(0.2, 0.8)));
	float rand = FMath::FRandRange(0.2, 0.8);
	StaticMeshComponent->SetWorldScale3D(FVector(rand, rand, rand));
}

