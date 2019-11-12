// Fill out your copyright notice in the Description page of Project Settings.
#include "LevelDesignGeneratedBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"

// Sets default values
ALevelDesignGeneratedBase::ALevelDesignGeneratedBase()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void ALevelDesignGeneratedBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelDesignGeneratedBase::OnConstruction(const FTransform& Transform)
{
	UpdateConstruction();
	
}

void ALevelDesignGeneratedBase::UpdateConstruction()
{
	if (IsValid(StaticMeshComponent->GetStaticMesh())) {
		FPositionVertexBuffer* vertexBuffer = &StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;

		if (vertexBuffer) {
			const int vertexCount = vertexBuffer->GetNumVertices();

			vertices.Empty();
			vertices.SetNum(vertexCount);

			for (int i = 0; i < vertexCount; i++) {
				const FVector worldSpaceVertexLocation = GetActorLocation() + GetTransform().TransformVector(vertexBuffer->VertexPosition(i));
				vertices[i] = worldSpaceVertexLocation;
				
			}
		}
	}
}
