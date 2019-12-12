// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshPassProcessor.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "SkywalkComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPowerStateChanged);


UCLASS(ClassGroup=(Custom),meta=(BlueprintSpawnableComponent),Blueprintable)
class PROJECT_OMBRES_API USkywalkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USkywalkComponent();

	UPROPERTY()
	ACharacter* Player;

	UPROPERTY(BlueprintAssignable)
	FPowerStateChanged OnSkywalkStart;

	UPROPERTY(BlueprintAssignable)
	FPowerStateChanged OnSkywalkAvailable;

	UPROPERTY(BlueprintAssignable)
	FPowerStateChanged OnSkywalkEnd;

	UPROPERTY()
	UParticleSystem* SkywalkVFX;

	UPROPERTY()
	UParticleSystemComponent* spawnedVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TilesSpawnProbability;


	UPROPERTY(EditAnywhere, Category = "Tweakable")
	float SkyWalkDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tweakable")
	float SkyWalkCoolDown;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float BringScrapDuration;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float PlaceScrapDuration;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float MaxRangeToGrabScrap;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float DistanceFromCamera;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float NoiseAmplitude;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float SpawnDistance;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	int ScrapsPerLine;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float ScrapsLevitationDuration;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float DistanceToGrabNewScraps;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float SpaceBetweenScraps;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float DistanceFromCamera2;

	UPROPERTY(EditAnywhere, Category = "NOT Tweakable")
	float BasePlatformAngle;

	UFUNCTION(BlueprintCallable)
	void StartSkyWalk();

	UFUNCTION(BlueprintCallable)
	void EndSkyWalk();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NOT Tweakable")
	FVector ScrapMiddlePosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NOT Tweakable")
	FVector ScrapRightOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NOT Tweakable")
	FVector ScrapFinalMiddlePosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NOT Tweakable")
	FVector ScrapMiddlePosition2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NOT Tweakable")
	FVector ScrapFinalMiddlePosition2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator TargetRotation;


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "NOT Tweakable")
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