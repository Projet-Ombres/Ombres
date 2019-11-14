// Fill out your copyright notice in the Description page of Project Settings.
#include "LevelDesignGeneratedBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

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
	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ALevelDesignGeneratedBase::DrawDebugStrings, 1, true);
}



void ALevelDesignGeneratedBase::UpdateConstruction()
{
	if (IsValid(StaticMeshComponent->GetStaticMesh())) {
		FPositionVertexBuffer* vertexBuffer = &StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
		FStaticMeshVertexBuffer* staticMeshVertexBuffer=&StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.StaticMeshVertexBuffer;

		if (vertexBuffer) {
			const int vertexCount = vertexBuffer->GetNumVertices();
			
			vertices.Empty();
			vertices.SetNum(vertexCount);

			binormals.Empty();
			binormals.SetNum(vertexCount);

			for (int i = 0; i < vertexCount; i++) {
				const FVector worldSpaceVertexLocation = GetActorLocation() + GetTransform().TransformVector(vertexBuffer->VertexPosition(i));
				binormals[i]=staticMeshVertexBuffer->VertexTangentY(i);
				vertices[i] = worldSpaceVertexLocation;
				
			}
		}
	}
}

void ALevelDesignGeneratedBase::DrawDebugStrings()
{
	/*for (int i = 0, l = vertices.Num(); i < l; i++) {

		DrawDebugString(GetWorld(), vertices[i], FString::FromInt(i), nullptr, FColor::Green, 2.0f, false);
	}*/
}

