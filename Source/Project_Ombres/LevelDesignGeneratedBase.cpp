// Fill out your copyright notice in the Description page of Project Settings.
#include "LevelDesignGeneratedBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Components/TextRenderComponent.h"


// Sets default values
ALevelDesignGeneratedBase::ALevelDesignGeneratedBase()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	textRendersParent = CreateDefaultSubobject<USceneComponent>(TEXT("TextRenders"));
	textRendersParent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ALevelDesignGeneratedBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelDesignGeneratedBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateConstruction();
}



void ALevelDesignGeneratedBase::UpdateConstruction()
{
	if (bRebake && IsValid(StaticMeshComponent->GetStaticMesh())) {
		FPositionVertexBuffer* vertexBuffer = &StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
		FStaticMeshVertexBuffer* staticMeshVertexBuffer=&StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.StaticMeshVertexBuffer;
		
		

		if (vertexBuffer) {
			const int vertexCount = vertexBuffer->GetNumVertices();
			
			vertices.Empty();
			vertices.SetNum(vertexCount);

			binormals.Empty();
			binormals.SetNum(vertexCount);


			while (textRendersParent->GetNumChildrenComponents() > 0) {
				textRendersParent->GetChildComponent(0)->DestroyComponent();
			}

			for (int i = 0; i < vertexCount; i++) {
				const FVector worldSpaceVertexLocation = GetActorLocation() + GetTransform().TransformVector(vertexBuffer->VertexPosition(i));
				binormals[i]=staticMeshVertexBuffer->VertexTangentY(i);
				vertices[i] = worldSpaceVertexLocation;
				UTextRenderComponent* textRender = NewObject<UTextRenderComponent>(this);
				textRender->AttachToComponent(textRendersParent, FAttachmentTransformRules::KeepRelativeTransform);
				textRender->SetWorldLocation(worldSpaceVertexLocation);
				textRender->SetWorldRotation(binormals[i].ToOrientationRotator());
				textRender->SetText(FText::FromString(FString::FromInt(i)));
				textRender->RegisterComponent();
				textRender->SetRelativeScale3D(FVector(0.01, 0.01, 0.01));
				textRender->SetTextRenderColor(textColor.ToFColor(false));
			}
		}
	}
}


