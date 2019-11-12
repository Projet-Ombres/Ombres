// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDesignBloc.h"
#include "Components/StaticMeshComponent.h"
#include "OmbresUtilities.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ALevelDesignBloc::ALevelDesignBloc()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
}



void ALevelDesignBloc::UpdateBlocPosition()
{
	if (IsValid(generatedLevel) && generatedLevel->vertices.Num()>0 && generatedLevel->vertices.IsValidIndex(Index)) {
		SetActorLocation(generatedLevel->vertices[Index]);
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


