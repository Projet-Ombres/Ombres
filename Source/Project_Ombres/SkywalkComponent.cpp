// Fill out your copyright notice in the Description page of Project Settings.


#include "SkywalkComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Engine/StaticMesh.h"
#include "SkywalkPlatform.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"


USkywalkComponent::USkywalkComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);
	

	SkyWalkDuration= 1.2;
	SkyWalkCoolDown = 10;
	BringScrapDuration = 0.5;
	PlaceScrapDuration = 0.3;
	MaxRangeToGrabScrap = 2000;
	DistanceFromCamera = 700;
	NoiseAmplitude = 100;
	SpawnDistance = 400;
	ScrapsPerLine = 3;
	ScrapsLevitationDuration = 2;
	DistanceToGrabNewScraps = 125;
	SpaceBetweenScraps = 100;
	DistanceFromCamera2 = 1400;
	BasePlatformAngle = 15;
	TilesSpawnProbability = 0.9;
	SpellEnabled = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> VFX(TEXT("/Game/Ombres/VFX/Skywalk/ParticleSystems/FX_Skywalk"));
	if (VFX.Succeeded()) {
		SkywalkVFX = VFX.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (staticMesh.Succeeded()) {
		ghostMesh = staticMesh.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> mat(TEXT("Material'/Game/Assets/SM/Cristal/Cristal_M.Cristal_M'"));
	if (mat.Succeeded()) {
		ghostMaterial = mat.Object;
	}


}


float USkywalkComponent::GetCurrentCoolDown()
{
	return CurrentCoolDown;
}

bool USkywalkComponent::GetOnCooldown()
{
	return OnCoolDown;
}

void USkywalkComponent::BeginPlay()
{
	Super::BeginPlay();
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CurrentCoolDown = SkyWalkCoolDown;
	OnCoolDown = false;

	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		if (Actor && Actor->ActorHasTag(TEXT("Scrap")))
		{
			ScrapsInWorld.Add(Actor);
		}
	}
}


void USkywalkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
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

	if (SpellEnabled) {
		if (!OnCoolDown) {
			Active = true;
			APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			cameraManager->ViewPitchMin = -45;
			cameraManager->ViewPitchMax = 45;
			Player->GetCharacterMovement()->MaxWalkSpeed = 500;

			CurrentCoolDown = 0;
			OnCoolDown = true;
			LastPlatformPosition = Player->GetActorLocation();
			currentTime = 0;

			spawnedVFX = UGameplayStatics::SpawnEmitterAttached(SkywalkVFX, Cast<USceneComponent>(Player->GetComponentByClass(UCameraComponent::StaticClass())), NAME_None, FVector(SpawnDistance, 0, -50), FRotator(25, 0, 0), FVector(1.5, 1.5, 1.5), EAttachLocation::SnapToTarget, true, EPSCPoolMethod::None);

			SetComponentTickEnabled(true);


			//spawn le fant�me
			GhostStaticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(LastPlatformPosition, cameraManager->GetCameraRotation());
			GhostStaticMeshActor->SetActorScale3D(FVector(5, 3, 0.5f));
			GhostStaticMeshActor->SetMobility(EComponentMobility::Movable);
			GhostStaticMeshActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			GhostStaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(ghostMesh);
			GhostStaticMeshActor->GetStaticMeshComponent()->SetMaterial(0, ghostMaterial);
			GhostStaticMeshActor->SetActorEnableCollision(false);

			OnSkywalkStart.Broadcast();
		}
	}

	
}

void USkywalkComponent::EndSkyWalk()
{
	if (Active) {
		SpawnPlatform(ScrapFinalMiddlePosition + FVector(0, 0, -75));
		SpawnPlatform(ScrapFinalMiddlePosition2 + FVector(0, 0, -75));


		Player->GetCharacterMovement()->MaxWalkSpeed = 765;
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		cameraManager->ViewPitchMin = -89.9;
		cameraManager->ViewPitchMax = 89.9;
		Active = false;
		OnCoolDown = true;

		ScrapsInUse.Empty();
		spawnedVFX->DestroyComponent();

		//on d�truit le fant�me � la fin du skywalk
		GhostStaticMeshActor->Destroy();
		
		OnSkywalkEnd.Broadcast();
	}
	
}



void USkywalkComponent::SetCoolDownTimer(float DeltaTime)
{
	if (!OnCoolDown) { return; }

	CurrentCoolDown += DeltaTime;
	
	


	if (CurrentCoolDown > SkyWalkCoolDown) {
		OnCoolDown = false;
		SetComponentTickEnabled(false);

		OnSkywalkAvailable.Broadcast();
	}
}

void USkywalkComponent::ResetCoolDown()
{
	CurrentCoolDown = SkyWalkCoolDown;
}

void USkywalkComponent::UpdateSkywalk()
{

	float completion = currentTime / SkyWalkDuration;
	UE_LOG(LogTemp, Warning, TEXT("completion : %f"), completion);


	//UE_LOG(LogTemp, Warning, TEXT("Update skywalk"));
	Player->GetCharacterMovement()->MaxWalkSpeed = 500;
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector playerPosition = Player->GetActorLocation();
	float speed = Player->GetVelocity().Size();

	FVector cameraPosition = cameraManager->GetCameraLocation();
	TargetRotation = cameraManager->GetCameraRotation();
	FVector cameraForwardVector = TargetRotation.Vector();
	FVector cameraRightVector = (cameraManager->GetCameraRotation() + FRotator(0, 90, 0)).Vector()*(ScrapsPerLine%2==0?SpaceBetweenScraps/2:SpaceBetweenScraps);
	FVector offsetVector = FVector(0, 0, -25);
	FVector ghostPosition = playerPosition + FVector(0, 0, -100) + cameraForwardVector * (500+300*(1-completion)*speed/760);
	FVector ghostScale = FVector(7 * (1 - completion) * speed/760, 3, 0.5f);

	ScrapFinalMiddlePosition2 = playerPosition + cameraForwardVector * 700 + offsetVector-cameraRightVector;
	ScrapFinalMiddlePosition = playerPosition + cameraForwardVector * 350 + offsetVector - cameraRightVector;
	ScrapMiddlePosition2 = cameraPosition + cameraForwardVector * DistanceFromCamera2- cameraRightVector;
	ScrapMiddlePosition = cameraPosition + cameraForwardVector * DistanceFromCamera- cameraRightVector;
	ScrapRightOffset = (cameraManager->GetCameraRotation() + FRotator(0, 90, 0)).Vector() * SpaceBetweenScraps;

	GhostStaticMeshActor->SetActorLocation(ghostPosition);
	GhostStaticMeshActor->SetActorRotation(TargetRotation);
	GhostStaticMeshActor->SetActorScale3D(ghostScale);
	

	if ((LastPlatformPosition - playerPosition).Size()>DistanceToGrabNewScraps) {
		LastPlatformPosition = playerPosition;
		ASkywalkPlatform* platform=SpawnPlatform(playerPosition + cameraForwardVector + FVector(0, 0, -100));
		
		platform->SpawnFirstLine();
		platform->SetDelay(0.1);	//Spawns second scrap line after a delay
	}


}

ASkywalkPlatform* USkywalkComponent::SpawnPlatform(FVector Position)
{
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FRotator cameraRotation=cameraManager->GetCameraRotation();
	FRotator targetRotation = FRotator(FMath::Clamp<float>(cameraRotation.Pitch + BasePlatformAngle, -40, 40), cameraRotation.Yaw, cameraRotation.Roll);

	ASkywalkPlatform* platform=Cast<ASkywalkPlatform>(GetWorld()->SpawnActor(ASkywalkPlatform::StaticClass(), &Position, &targetRotation));
	platform->FadeTime = ScrapsLevitationDuration + BringScrapDuration + PlaceScrapDuration;
	platform->skywalkComponent = this;
	platform->BringScrapDuration = BringScrapDuration;
	platform->ScrapLifeTime = ScrapsLevitationDuration;
	platform->PlaceScrapDuration = PlaceScrapDuration;
	platform->NoiseAmplitude = NoiseAmplitude;
	platform->SpawnDistance = SpawnDistance;
	platform->Init();
	LastPlatformSpawned = platform;
	return platform;
}

void USkywalkComponent::ReleaseScraps()
{
	if (Active) {
		ScrapsInWorld.Append(ScrapsInUse);
		ScrapsInUse.Empty();
	}
}

AActor* USkywalkComponent::GetClosestScrap()
{
	AActor* nearest = nullptr;

	float minDist = TNumericLimits<float>::Max();

	for (int i = 0, l = ScrapsInWorld.Num(); i < l; i++) {
		AActor* actor = ScrapsInWorld[i];
		if (actor == NULL) {
			ScrapsInWorld.RemoveAt(i);
			i--;
			l--;
			continue;
		}

		if (!ScrapsInUse.Contains(actor)) {
			if (actor != NULL && Player != NULL) {
				float distance = FVector::Dist(Player->GetActorLocation(), actor->GetActorLocation());

				if (distance < minDist && distance < MaxRangeToGrabScrap) {
					nearest = actor;
					minDist = distance;
				}
			}
			
		}
	}
	return nearest;
}


void USkywalkComponent::FinishSkywalk()
{
	SpawnPlatform(LastPlatformPosition + FVector(0, 0, -100));
	ReleaseScraps();
	EndSkyWalk();
}



