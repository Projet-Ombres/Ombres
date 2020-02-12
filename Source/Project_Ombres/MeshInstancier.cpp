// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshInstancier.h"

// Sets default values
AMeshInstancier::AMeshInstancier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMeshInstancier::BeginPlay()
{
	Super::BeginPlay();
	
}

UHierarchicalInstancedStaticMeshComponent* AMeshInstancier::AddHierarchicalComponent()
{
	HierarchicalInstancedStaticMeshComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, UHierarchicalInstancedStaticMeshComponent::StaticClass());
	HierarchicalInstancedStaticMeshComponent->RegisterComponent();
	HierarchicalInstancedStaticMeshComponent->SetupAttachment(GetRootComponent());
	return HierarchicalInstancedStaticMeshComponent;
}

