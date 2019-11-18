// Fill out your copyright notice in the Description page of Project Settings.


#include "SkywalkComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Engine/StaticMesh.h"
#include "Scrap.h"
#include "SkywalkPlatform.h"
#include "Kismet/GameplayStatics.h"


USkywalkComponent::USkywalkComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void USkywalkComponent::BeginPlay()
{
	Super::BeginPlay();
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CurrentCoolDown = SkyWalkCoolDown;
}


void USkywalkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void USkywalkComponent::StartSkyWalk()
{
	Player->GetCharacterMovement()->MaxWalkSpeed = 500;
	Active = true;
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	cameraManager->ViewPitchMin = -45;
	cameraManager->ViewPitchMax = 45;

	if (!OnCoolDown) {
		CurrentCoolDown = 0;
		OnCoolDown = true;
		LastPlatformPosition = Player->GetActorLocation();
		UCustomTimeline::StartCustomTimeline(GetWorld(), 1 / SkyWalkDuration, 0, timelineOutputValue, timelineDeltaTime, Timeline);

		FScriptDelegate UpdateDelegate;
		UpdateDelegate.BindUFunction(this, FName("UpdateSkywalk"));
		Timeline->Update.Add(UpdateDelegate);

		FScriptDelegate FinishedDelegate;
		FinishedDelegate.BindUFunction(this, FName("FinishSkywalk"));
		Timeline->Finished.Add(FinishedDelegate);
	}
}

void USkywalkComponent::EndSkyWalk()
{
	Player->GetCharacterMovement()->MaxWalkSpeed = 900;
	APlayerCameraManager* cameraManager=UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	cameraManager->ViewPitchMin = -90;
	cameraManager->ViewPitchMax = 90;
	Active = false;
	OnCoolDown = true;
	UCustomTimeline::StopCustomTimeline(this, Timeline);
	ScrapsInUse.Empty();
}



void USkywalkComponent::SetCoolDownTimer(float DeltaTime)
{
	CurrentCoolDown += DeltaTime;
	OnCoolDown = (CurrentCoolDown >= SkyWalkCoolDown);
}

void USkywalkComponent::ResetCoolDown()
{
	CurrentCoolDown = SkyWalkCoolDown;
}

void USkywalkComponent::UpdateSkywalk()
{
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector playerPosition = Player->GetActorLocation();
	FVector cameraPosition = cameraManager->GetCameraLocation();
	FVector cameraForwardVector = cameraManager->GetCameraRotation().Vector();
	FVector offsetVector = FVector(0, 0, -25);

	ScrapFinalMiddlePosition2 = playerPosition + cameraForwardVector * 700 + offsetVector;
	ScrapFinalMiddlePosition = playerPosition + cameraForwardVector * 350 + offsetVector;
	ScrapMiddlePosition2 = cameraPosition + cameraForwardVector * DistanceFromCamera2;
	ScrapMiddlePosition = cameraPosition + cameraForwardVector * DistanceFromCamera;
	ScrapRightOffset = (cameraManager->GetCameraRotation() + FRotator(0, 90, 0)).Vector() * SpaceBetweenScraps;

	if ((LastPlatformPosition - playerPosition).Size()) {
		LastPlatformPosition = playerPosition;
		SpawnPlatform(playerPosition + cameraForwardVector + FVector(0, 0, -100));
		
		SpawnScrapsLine();
		SpawnPlatform(ScrapFinalMiddlePosition + FVector(0, 0, -75));
		GetWorld()->GetTimerManager().SetTimer(secondLineTimerHandle, this, &USkywalkComponent::WaitForZeroPointTwoSeconds, 5, false);


	}


}

void USkywalkComponent::SpawnPlatform(FVector Position)
{
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FRotator cameraRotation=cameraManager->GetCameraRotation();
	FRotator targetRotation = FRotator(FMath::Clamp<float>(cameraRotation.Pitch, -40, 40) + BasePlatformAngle, cameraRotation.Yaw, cameraRotation.Roll);

	ASkywalkPlatform* platform=Cast<ASkywalkPlatform>(GetWorld()->SpawnActor(ASkywalkPlatform::StaticClass(), &Position, &targetRotation));
	platform->FadeTime = ScrapsLevitationDuration + BringScrapDuration + PlaceScrapDuration;
	LastPlatformSpawned = platform;
	
}

void USkywalkComponent::ReleaseScraps()
{
	ScrapsInWorld.Append(ScrapsInUse);
	ScrapsInUse.Empty();
}

AActor* USkywalkComponent::GetClosestScrap()
{
	AActor* nearest = nullptr;

	float minDist = TNumericLimits<float>::Max();

	for (int i = 0, l = ScrapsInWorld.Num(); i < l; i++) {
		AActor* actor = ScrapsInWorld[i];
		if (!ScrapsInUse.Contains(actor)) {
			float distance = FVector::Dist(Player->GetActorLocation(), actor->GetActorLocation());

			if (distance < minDist && distance < MaxRangeToGrabScrap) {
				nearest = actor;
				minDist = distance;
			}
		}
	}
	return nearest;

}

void USkywalkComponent::MoveClosestScrap(int LineIndex,int Line)
{
	NearestScrap=GetClosestScrap();
	if (IsValid(NearestScrap)) {
		UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(NearestScrap->GetComponentByClass(UStaticMesh::StaticClass()));
		staticMeshComponent->SetSimulatePhysics(false);
		staticMeshComponent->AttachToComponent(NearestScrap->GetRootComponent(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::KeepWorld,EAttachmentRule::KeepWorld,true));
		staticMeshComponent->SetEnableGravity(false);
		staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		ScrapsInWorld.Remove(NearestScrap);
		MoveScrap(LineIndex, Line);
	
	}
	else {
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FVector cameraPosition=cameraManager->GetCameraLocation();
		FVector cameraRightVector = cameraManager->GetCameraRotation().Vector().RightVector;
		FVector pos = cameraPosition + (cameraRightVector * (FMath::Rand() / RAND_MAX * 2 - 1) * SpawnDistance);


		FVector targetPosition= FVector(pos.X, pos.Y, pos.Z - 150);


		NearestScrap=GetWorld()->SpawnActor(AScrap::StaticClass(), &targetPosition);
		MoveScrap(LineIndex, Line);
	}
}

void USkywalkComponent::SpawnScrapsLine()
{
	for (int i = 0; i < ScrapsPerLine; i++) {
		MoveClosestScrap(i, 0);
	}
}

void USkywalkComponent::SpawnSecondScrapsLine()
{
	for (int i = 0; i < ScrapsPerLine; i++) {
		MoveClosestScrap(i, 1);
	}
}

void USkywalkComponent::MoveScrap(int LineIndex, int Line)
{
	Cast<AScrap>(NearestScrap)->StartMovingToTarget(NoiseAmplitude, Player, BringScrapDuration, PlaceScrapDuration, LineIndex-(ScrapsPerLine/2), Line, ScrapsLevitationDuration);
	ScrapsInUse.Add(NearestScrap);
}

void USkywalkComponent::FinishSkywalk()
{
	SpawnPlatform(LastPlatformPosition);
	ReleaseScraps();
	EndSkyWalk();
}

void USkywalkComponent::WaitForZeroPointTwoSeconds()
{
	SpawnSecondScrapsLine();
	SpawnPlatform(ScrapFinalMiddlePosition2 + FVector(0, 0, -75));
}

