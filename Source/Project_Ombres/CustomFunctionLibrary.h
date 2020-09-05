// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "ConstructorHelpers.h"
#include "Misc/Char.h"
#include <string>
#include <algorithm>
#include "StaticMeshResources.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "CustomFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FRecordInfo {
	GENERATED_BODY()
	FRecordInfo() {
		LevelName = "None";
		CheckpointId = -1;
		Date = FDateTime();
		FileName = "None";
	}
	UPROPERTY(BlueprintReadWrite)
	FString LevelName;
	UPROPERTY(BlueprintReadWrite)
	FDateTime Date;
	UPROPERTY(BlueprintReadWrite)
	FString FileName;
	UPROPERTY(BlueprintReadWrite)
	int CheckpointId;
};

/**
 * 
 */
UCLASS()
class PROJECT_OMBRES_API UCustomFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
		static void SetCollisionResponseToChannels(UPrimitiveComponent* Component, FCollisionResponseContainer collision);

	UFUNCTION(BlueprintCallable)
		static bool CollisionEquals(FCollisionResponseContainer collision1, FCollisionResponseContainer collision2);


	UFUNCTION(BlueprintCallable)
		static FCollisionResponseContainer GetCollisionResponseToChannels(UPrimitiveComponent* Component);


	UFUNCTION(BlueprintCallable)
		static TArray<FString> GetRecordContent(FString FileName);

	UFUNCTION(BlueprintCallable)
	static void SetLightmapResolution(UStaticMesh* StaticMesh,int Resolution) {
		StaticMesh->LightMapResolution = Resolution;
	}

	UFUNCTION(BlueprintCallable)
	static void RebuildStaticMesh(UStaticMesh* StaticMesh) {
		StaticMesh->Build();
	}




	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GamepadConnected"), Category = "System Information")
		static bool IsGamePadConnected();

	UFUNCTION(BlueprintCallable)
		static TArray<FRecordInfo> GetRecordsInfo();
	

	UFUNCTION(BlueprintCallable,BlueprintPure)
	static const FString CorrectJson(FString string) {
		string.RemoveSpacesInline();
		std::string stdString(TCHAR_TO_UTF8(*string));
		
		stdString.erase(std::remove(stdString.begin(), stdString.end(), '\n'), stdString.end());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(stdString.c_str()));
		return FString(stdString.c_str());
	}


	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FString ConvertToUTF8(FString s) {
		s.Replace(TEXT("u00e"), TEXT("\\u00e"));
		return s;

	}

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable)
	static void SetLODGroup(UStaticMesh* staticMesh,FName LODGroup) {
		staticMesh->SetLODGroup(LODGroup);
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
