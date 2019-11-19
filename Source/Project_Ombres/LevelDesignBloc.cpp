// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDesignBloc.h"
#include "Components/StaticMeshComponent.h"
#include "OmbresUtilities.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ALevelDesignBloc::ALevelDesignBloc()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetRelativeLocation(FVector(0, 0, 0));
	StaticMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
}



void ALevelDesignBloc::UpdateBlocPosition()
{
	if (IsValid(generatedLevel) && generatedLevel->vertices.Num()>0 && generatedLevel->vertices.IsValidIndex(Index)) {
		FVector position = generatedLevel->vertices[Index];
		FRotator rotation = generatedLevel->binormals[Index].ToOrientationRotator();
		//UKismetSystemLibrary::DrawDebugArrow(GetWorld(), position, position + generatedLevel->binormals[Index] * 100,10,FLinearColor::Blue,5,1);
		SetActorLocation(position);
		SetActorRotation(rotation);

	}
}

void ALevelDesignBloc::UpdateBlocMesh()
{
	UStaticMesh* staticMesh = NULL;
	if (Randomize) {
		staticMesh = GetRandomMesh();
	}
	else {
		 staticMesh= GetMeshByBlocType(BlocType);
	}

	if (staticMesh != NULL) {
		StaticMeshComponent->SetStaticMesh(staticMesh);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Static mesh not found"));
	}
	
}

UStaticMesh* ALevelDesignBloc::GetMeshByBlocType(ELDBlocType_10x10 blocType)
{
	UStaticMesh* staticMesh=NULL;
	for (int i = 0, l = meshes.Num(); i < l; i++) {
		if (meshes[i].blocType == blocType) {
			staticMesh = meshes[i].staticMesh;
			break;
		}
	}
	return staticMesh;
}



UStaticMesh* ALevelDesignBloc::GetRandomMesh() {
	UStaticMesh* staticMesh = NULL;
	
	if (meshes.Num() > 0) {
		int rand = FMath::RandRange(0, meshes.Num() - 1);
		staticMesh = meshes[rand].staticMesh;
	}
	return staticMesh;
}


