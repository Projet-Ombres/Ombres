// Fill out your copyright notice in the Description page of Project Settings.


#include "SkywalkComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Engine/StaticMesh.h"
#include "SkywalkPlatform.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"


USkywalkComponent::USkywalkComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);

	SkyWalkDuration= 0.5;
	SkyWalkCoolDown = 10;
	BringScrapDuration = 0.5;
	PlaceScrapDuration = 0.3;
	MaxRangeToGrabScrap = 2000;
	DistanceFromCamera = 700;
	NoiseAmplitude = 100;
	SpawnDistance = 200;
	ScrapsPerLine = 3;
	ScrapsLevitationDuration = 3;
	DistanceToGrabNewScraps = 125;
	SpaceBetweenScraps = 100;
	DistanceFromCamera2 = 1400;
	BasePlatformAngle = 25;
	TilesSpawnProbability = 0.9;
	SpellEnabled = true;
	platformsToSpawnCount = 7;
	WalkSpeed = 150;

	VFXScale = FVector(0.2f, 0.2f, 0.2f);
	VFXRotation = FRotator(0, 0, -90);

	EnablePreviews = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> VFX(TEXT("/Game/Ombres/VFX/Skywalk/ParticleSystems/FX_Skywalk"));
	if (VFX.Succeeded()) {
		SkywalkVFX = VFX.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> mat(TEXT("Material'/Game/Assets/SM/Cristal/Cristal_M.Cristal_M'"));

	if (mat.Succeeded()) {
		PreviewMaterial = mat.Object;
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

	CameraManager= UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
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
			timeLeftToSpawnPlatform -= DeltaTime;
			UpdateSkywalk();
		}

		
	}
	SetCoolDownTimer(DeltaTime);
}


void USkywalkComponent::StartSkyWalk()
{
	if (SpellEnabled) {
		if (!OnCoolDown) {
			platformsSpawned = 0;
			SortScrapsInWorld();
			GeneratePropsTypes();
			CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
			FRotator cameraRotation = CameraManager->GetCameraRotation();

			TargetRotation = FRotator(FMath::Clamp<float>(cameraRotation.Pitch + BasePlatformAngle, -40, 40), cameraRotation.Yaw, cameraRotation.Roll);

			Active = true;
			CameraManager->ViewPitchMin = -45;
			CameraManager->ViewPitchMax = 45;
			Player->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

			CurrentCoolDown = 0;
			OnCoolDown = true;
			LastPlatformPosition = Player->GetActorLocation();
			currentTime = 0;

			spawnedVFX = UGameplayStatics::SpawnEmitterAttached(SkywalkVFX, Cast<USkeletalMeshComponent>(Player->GetComponentByClass(USkeletalMeshComponent::StaticClass())), FName("hand_r"), FVector(0, 0, 0), VFXRotation, VFXScale, EAttachLocation::SnapToTarget, true, EPSCPoolMethod::None);

			SetComponentTickEnabled(true);
			OnSkywalkStart.Broadcast();
		}
	}

	
}

void USkywalkComponent::EndSkyWalk()
{
	if (Active) {

		/*FVector cameraForwardVector = cameraManager->GetCameraRotation().Vector();
		SpawnPlatform(ScrapFinalMiddlePosition + FVector(0,0,-100) );
		SpawnPlatform(ScrapFinalMiddlePosition2 + FVector(0,0,-100) );*/

		Player->GetCharacterMovement()->MaxWalkSpeed = 765;
		Player->GetCharacterMovement()->Velocity = Player->GetActorForwardVector() * 765;
		CameraManager->ViewPitchMin = -89.9;
		CameraManager->ViewPitchMax = 89.9;
		Active = false;
		OnCoolDown = true;

		ScrapsInUse.Empty();
		spawnedVFX->DestroyComponent();
		
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
	//UE_LOG(LogTemp, Warning, TEXT("Update skywalk"));
	Player->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	FVector playerPosition = Player->GetActorLocation();
	
	FVector cameraForwardVector = TargetRotation.Vector();

	float distanceBetweenPlatforms = SpawnDistance * platformsSpawned;



	
	if (timeLeftToSpawnPlatform<=0) {
		timeLeftToSpawnPlatform = SkyWalkDuration / platformsToSpawnCount;
		LastPlatformPosition = playerPosition;
		ASkywalkPlatform* platform=SpawnPlatform(playerPosition + cameraForwardVector* distanceBetweenPlatforms + FVector(0, 0, -100));
		
		platform->SpawnFirstLine();
		platform->SetDelay(0.1);	//Spawns second scrap line after a delay

		platformsSpawned++;
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
	if (ScrapsInWorld.Num() > 0) {
		return ScrapsInWorld[0];
	}
	else {
		return nullptr;
	}
}


void USkywalkComponent::FinishSkywalk()
{
	SpawnPlatform(LastPlatformPosition + FVector(0, 0, -100));
	ReleaseScraps();
	EndSkyWalk();
}

void USkywalkComponent::GeneratePropsTypes()
{
	propsTypes.Empty();
	propsTypes.SetNum(100);

	for (int i = 0; i < 100; i++) {
		propsTypes[i] = FMath::RandRange(0, 7);
	}
}


void USkywalkComponent::SortScrapsInWorld()
{
	TArray<FScrapWithDistance> scrapsWithDistance;
	FVector playerLocation = Player->GetActorLocation();

	for (int i =0,l=ScrapsInWorld.Num();i<l;i++){
		AActor* Actor = ScrapsInWorld[i];
		if (IsValid(Actor)) {
			FScrapWithDistance scrapWithDistance;
			scrapWithDistance.scrap = Actor;
			scrapWithDistance.distance = UKismetMathLibrary::Vector_Distance(Actor->GetActorLocation(), playerLocation);
			scrapsWithDistance.Add(scrapWithDistance);
		}
	}

	TArray<FScrapWithDistance> sortedScraps;

	int index = 0;
	int scrapsNum = scrapsWithDistance.Num();
	while (index < scrapsNum) {
		FScrapWithDistance scrapWithDist = scrapsWithDistance[index];
		for (int i = 0, l = scrapsWithDistance.Num(); i < l; i++) {
			if (i >= index) {
				sortedScraps.Add(scrapWithDist);
				index++;
				break;
			}
			else {
				if (scrapWithDist.distance > MaxRangeToGrabScrap) {
					index++;
					break;
				}
				if (i<sortedScraps.Num() && scrapWithDist.distance < sortedScraps[i].distance) {
					sortedScraps.Insert(scrapWithDist, i);
					index++;
					break;
				}
			}
		}
	}

	ScrapsInWorld.SetNum(sortedScraps.Num());
	for (int i = 0; i < sortedScraps.Num(); i++) {
		ScrapsInWorld[i] = sortedScraps[i].scrap;
	}
}