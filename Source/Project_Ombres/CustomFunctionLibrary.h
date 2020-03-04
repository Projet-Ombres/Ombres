// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "ConstructorHelpers.h"
#include "CustomFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_OMBRES_API UCustomFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:

	template <typename Key>
	UFUNCTION(BlueprintCallable)
	static FORCEINLINE TMap<Key,float>* SetMapElement(TMap<Key, float> &Map, Key key,float value) {
		Map[key] = value;
		return Map;
	}

#if WITH_EDITOR


	UFUNCTION(BlueprintCallable)
	static void GenerateLevelArchitectureLOD(UStaticMesh* staticMesh) {
		staticMesh->SetLODGroup(FName(TEXT("LevelArchitecture")));
	}

	UFUNCTION(BlueprintCallable)
	static UChildActorComponent* AddChildActor(AActor* target) {
		UChildActorComponent* actorComponent = NewObject<UChildActorComponent>(target, UChildActorComponent::StaticClass(), FName("ChildActorComponent"));
		actorComponent->SetChildActorClass(target->GetClass());
		actorComponent->CreateChildActor();
		actorComponent->SetupAttachment(target->GetRootComponent());
		actorComponent->RegisterComponent();
		return actorComponent;
	}


	UFUNCTION(BlueprintCallable)
	static void ReplaceDefaultMaterials(TArray<UStaticMesh*> SelectedMeshes) {
		FSoftObjectPath matPath = FSoftObjectPath(TEXT("/Game/Materials/M_Black"));
		UMaterialInterface* mat=Cast<UMaterialInterface>(matPath.TryLoad());
		if (mat ==nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Couldn't find material base material (%s)"),*mat->GetName()); 
			return;
		}


		for (int i = 0, l = SelectedMeshes.Num(); i < l; i++) {
			UStaticMesh* staticMesh = SelectedMeshes[i];
			
			if (staticMesh->GetNumLODs() > 0) {
				for (int j = 0, ll = staticMesh->Materials_DEPRECATED.Num(); j < ll; j++) {
					UMaterialInterface* materialInterface = staticMesh->GetMaterial(j);
					if (materialInterface != nullptr) {
						if (materialInterface->GetMaterial()->IsDefaultMaterial()) {
							UE_LOG(LogTemp, Warning, TEXT("Replaced default material in %s"),*staticMesh->GetName());
							staticMesh->SetMaterial(j, mat);
						}

					}
				}
			}

			

		}
	}
#endif



};
