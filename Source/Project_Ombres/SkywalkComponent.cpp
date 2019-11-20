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
	SetComponentTickEnabled(false);

	SkyWalkDuration= 3;
	SkyWalkCoolDown = 10;
	BringScrapDuration = 0.3;
	PlaceScrapDuration = 0.15;
	MaxRangeToGrabScrap = 2000;
	DistanceFromCamera = 700;
	NoiseAmplitude = 100;
	SpawnDistance = 400;
	ScrapsPerLine = 2;
	ScrapsLevitationDuration = 2;
	DistanceToGrabNewScraps = 120;
	SpaceBetweenScraps = 100;
	DistanceFromCamera2 = 1400;
	BasePlatformAngle = 15;

}


void USkywalkComponent::BeginPlay()
{
	Super::BeginPlay();
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CurrentCoolDown = SkyWalkCoolDown;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AScrap::StaticClass(), ScrapsInWorld);
}


void USkywalkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Warning, TEXT("Active : %s"), (Active ? TEXT("True") : TEXT("False")));
	
	if (Active) {
		currentTime += DeltaTime;


		if (currentTime >= SkyWalkDuration) {
			FinishSkywalk();
		}
		else {
			UpdateSkywalk();
		}

		
	}
	SetCoolDownTimer(DeltaTime);
}


void USkywalkComponent::StartSkyWalk()
{
	if (!OnCoolDown) {
		Player->GetCharacterMovement()->MaxWalkSpeed = 500;
		Active = true;
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		cameraManager->ViewPitchMin = -45;
		cameraManager->ViewPitchMax = 45;

		UE_LOG(LogTemp, Warning, TEXT("Start skywalk"));

		CurrentCoolDown = 0;
		OnCoolDown = true;
		LastPlatformPosition = Player->GetActorLocation();
		currentTime = 0;
		SetComponentTickEnabled(true);
	
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
	SetComponentTickEnabled(false);
	ScrapsInUse.Empty();
}



void USkywalkComponent::SetCoolDownTimer(float DeltaTime)
{
	if (!OnCoolDown) { return; }

	CurrentCoolDown += DeltaTime;

	if (CurrentCoolDown > SkyWalkCoolDown) {
		OnCoolDown = false;
	}
}

void USkywalkComponent::ResetCoolDown()
{
	CurrentCoolDown = SkyWalkCoolDown;
}

void USkywalkComponent::UpdateSkywalk()
{
	//UE_LOG(LogTemp, Warning, TEXT("Update skywalk"));

	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector playerPosition = Player->GetActorLocation();
	FVector cameraPosition = cameraManager->GetCameraLocation();
	FVector cameraForwardVector = cameraManager->GetCameraRotation().Vector();
	FVector cameraRightVector = (cameraManager->GetCameraRotation() + FRotator(0, 90, 0)).Vector()*30;
	FVector offsetVector = FVector(0, 0, -25);

	ScrapFinalMiddlePosition2 = playerPosition + cameraForwardVector * 700 + offsetVector-cameraRightVector;
	ScrapFinalMiddlePosition = playerPosition + cameraForwardVector * 350 + offsetVector - cameraRightVector;
	ScrapMiddlePosition2 = cameraPosition + cameraForwardVector * DistanceFromCamera2- cameraRightVector;
	ScrapMiddlePosition = cameraPosition + cameraForwardVector * DistanceFromCamera- cameraRightVector;
	ScrapRightOffset = (cameraManager->GetCameraRotation() + FRotator(0, 90, 0)).Vector() * SpaceBetweenScraps;

	if ((LastPlatformPosition - playerPosition).Size()>DistanceToGrabNewScraps) {
		LastPlatformPosition = playerPosition;
		ASkywalkPlatform* platform=SpawnPlatform(playerPosition + cameraForwardVector + FVector(0, 0, -100));
		
		SpawnScrapsLine();
		//SpawnPlatform(ScrapFinalMiddlePosition + FVector(0, 0, -75));
		platform->SetDelay(0.1);	//Spawns second scrap line after a delay
	}


}

ASkywalkPlatform* USkywalkComponent::SpawnPlatform(FVector Position)
{
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FRotator cameraRotation=cameraManager->GetCameraRotation();
	FRotator targetRotation = FRotator(FMath::Clamp<float>(cameraRotation.Pitch, -40, 40) + BasePlatformAngle, cameraRotation.Yaw, cameraRotation.Roll);

	ASkywalkPlatform* platform=Cast<ASkywalkPlatform>(GetWorld()->SpawnActor(ASkywalkPlatform::StaticClass(), &Position, &targetRotation));
	platform->FadeTime = ScrapsLevitationDuration + BringScrapDuration + PlaceScrapDuration;
	platform->skywalkComponent = this;
	platform->Init();
	LastPlatformSpawned = platform;
	return platform;
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
		UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(NearestScrap->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (IsValid(staticMeshComponent)) {
			staticMeshComponent->SetSimulatePhysics(false);
			staticMeshComponent->AttachToComponent(NearestScrap->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
			staticMeshComponent->SetEnableGravity(false);
			staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			ScrapsInWorld.Remove(NearestScrap);
			MoveScrap(LineIndex, Line);
		}
		
	
	}
	else {
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FVector cameraPosition=cameraManager->GetCameraLocation();
		FVector cameraRightVector = cameraManager->GetCameraRotation().Vector().RightVector;
		FVector pos = cameraPosition + (cameraRightVector * (FMath::Rand() / RAND_MAX * 2 - 1) * SpawnDistance);


		FVector targetPosition= FVector(pos.X, pos.Y, 150);


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
	Cast<AScrap>(NearestScrap)->StartMovingToTarget(NoiseAmplitude, Player, BringScrapDuration, PlaceScrapDuration, LineIndex-(ScrapsPerLine/2)+1, Line, ScrapsLevitationDuration);
	ScrapsInUse.Add(NearestScrap);
}

void USkywalkComponent::FinishSkywalk()
{
	SpawnPlatform(LastPlatformPosition + FVector(0, 0, -75));
	ReleaseScraps();
	EndSkyWalk();
}

void USkywalkComponent::WaitForZeroPointTwoSeconds()
{
	SpawnSecondScrapsLine();
	SpawnPlatform(ScrapFinalMiddlePosition2 + FVector(0, 0, -75));
}

