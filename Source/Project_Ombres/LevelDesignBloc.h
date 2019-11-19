// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "LevelDesignGeneratedBase.h"
#include "LevelDesignBloc.generated.h"

UENUM(BlueprintType)
enum class ELDBlocType_10x10 : uint8 {
	LDBT_10x10_Simple,
	LDBT_10x10_NotSoSimple,
	LDBT_10x10_ABitComplex,
	LDBT_10x10_Complicated
};

USTRUCT(BlueprintType)
struct FMeshByBlocType_10x10 {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ELDBlocType_10x10 blocType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* staticMesh;
};

UCLASS()
class PROJECT_OMBRES_API ALevelDesignBloc : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelDesignBloc();

	UPROPERTY(EditInstanceOnly)
	ALevelDesignGeneratedBase* generatedLevel;

	UPROPERTY(EditInstanceOnly)
	bool Randomize;

	UPROPERTY(EditInstanceOnly)
	int Index;

	UPROPERTY(EditInstanceOnly)
	ELDBlocType_10x10 BlocType;

	UPROPERTY(EditDefaultsOnly)
	TArray<FMeshByBlocType_10x10> meshes;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;



private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override {
		Super::PostEditChangeProperty(PropertyChangedEvent);
		FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelDesignBloc,Index)  || (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelDesignBloc, generatedLevel))) {
			UpdateBlocPosition();
		}
		else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelDesignBloc, BlocType) ||PropertyName == GET_MEMBER_NAME_CHECKED(ALevelDesignBloc, Randomize)) {
			UpdateBlocMesh();
		}
		
	};
#endif
	void UpdateBlocPosition();

	void UpdateBlocMesh();

	UStaticMesh* GetMeshByBlocType(ELDBlocType_10x10 blocType);

	UStaticMesh* GetRandomMesh();
};
