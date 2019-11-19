// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshPassProcessor.h"
#include "GameFramework/Character.h"
#include "SkywalkComponent.generated.h"


UCLASS(ClassGroup=(Custom),meta=(BlueprintSpawnableComponent),Blueprintable)
class PROJECT_OMBRES_API USkywalkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkywalkComponent();

	UPROPERTY()
	ACharacter* Player;

	UPROPERTY(EditAnywhere)
	float SkyWalkDuration;

	UPROPERTY(EditAnywhere)
	float SkyWalkCoolDown;

	UPROPERTY(EditAnywhere)
	float BringScrapDuration;

	UPROPERTY(EditAnywhere)
	float PlaceScrapDuration;

	UPROPERTY(EditAnywhere)
	float MaxRangeToGrabScrap;

	UPROPERTY(EditAnywhere)
	float DistanceFromCamera;

	UPROPERTY(EditAnywhere)
	float NoiseAmplitude;

	UPROPERTY(EditAnywhere)
	float SpawnDistance;

	UPROPERTY(EditAnywhere)
	int ScrapsPerLine;

	UPROPERTY(EditAnywhere)
	float ScrapsLevitationDuration;

	UPROPERTY(EditAnywhere)
	float DistanceToGrabNewScraps;

	UPROPERTY(EditAnywhere)
	float SpaceBetweenScraps;

	UPROPERTY(EditAnywhere)
	float DistanceFromCamera2;

	UPROPERTY(EditAnywhere)
	float BasePlatformAngle;

	UFUNCTION(BlueprintCallable)
	void StartSkyWalk();

	UFUNCTION(BlueprintCallable)
	void EndSkyWalk();

	UPROPERTY()
	FVector ScrapMiddlePosition;

	UPROPERTY()
	FVector ScrapRightOffset;

	UPROPERTY()
	FVector ScrapFinalMiddlePosition;

	UPROPERTY()
	FVector ScrapMiddlePosition2;

	UPROPERTY()
	FVector ScrapFinalMiddlePosition2;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool Active;

	UFUNCTION(BlueprintCallable)
	void ReleaseScraps();


protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	FVector LastPlatformPosition;

	UPROPERTY()
	bool OnCoolDown;

	UPROPERTY()
	float CurrentCoolDown;

	UPROPERTY()
	TArray<AActor*> ScrapsInWorld;

	UPROPERTY()
	TArray<AActor*> ScrapsInUse;

	UPROPERTY()
	AActor* NearestScrap;

	

	UPROPERTY()
	AActor* LastPlatformSpawned;


	UFUNCTION()
	void SetCoolDownTimer(float DeltaTime);

	UFUNCTION()
	void ResetCoolDown();

	UFUNCTION()
	void UpdateSkywalk();

	UFUNCTION()
	void SpawnPlatform(FVector Position);

	

	UFUNCTION()
	AActor* GetClosestScrap();

	UFUNCTION()
	void MoveClosestScrap(int LineIndex,int Line);

	UFUNCTION()
	void SpawnScrapsLine();

	UFUNCTION()
	void SpawnSecondScrapsLine();

	UFUNCTION()
	void MoveScrap(int LineIndex, int Line);

	UFUNCTION()
	void FinishSkywalk();

	UFUNCTION()
	void WaitForZeroPointTwoSeconds();


	float currentTime;
	FTimerHandle secondLineTimerHandle;

};
/*
struct FECS_System {
	virtual void update(entt::registry& registry,float dt) {

	}
};


struct FEntityTransform {
	FTransform transform;
};

struct FEntityMesh {
	UStaticMesh* staticMesh;
};


struct FTestComponent : FECS_System {
	void update(entt::registry& registry,float dt) override {
		registry.view<FEntityTransform, FEntityMesh>().each([&, dt](auto entity, FEntityTransform& transform, FEntityMesh& mesh) {
			
		});
	}
};*/