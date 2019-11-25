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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ScrapMiddlePosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ScrapRightOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ScrapFinalMiddlePosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ScrapMiddlePosition2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ScrapFinalMiddlePosition2;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool Active;

	UFUNCTION(BlueprintCallable)
	void ReleaseScraps();

	UFUNCTION()
	AActor* GetClosestScrap();

	UFUNCTION(BlueprintCallable)
	void ResetCoolDown();

	UPROPERTY()
	TArray<AActor*> ScrapsInWorld;

	UPROPERTY()
	TArray<AActor*> ScrapsInUse;

	UFUNCTION()
	void SetCoolDownTimer(float DeltaTime);

	UPROPERTY(BlueprintReadOnly)
	AActor* LastPlatformSpawned;

	UFUNCTION(BlueprintPure)
	float GetCurrentCoolDown();

	UFUNCTION(BlueprintPure)
	bool GetOnCooldown();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	FVector LastPlatformPosition;

	

	UFUNCTION()
	void UpdateSkywalk();

	UFUNCTION()
	class ASkywalkPlatform* SpawnPlatform(FVector Position);

	UFUNCTION()
	void FinishSkywalk();

	UPROPERTY()
	float CurrentCoolDown;

	UPROPERTY()
	bool OnCoolDown;



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