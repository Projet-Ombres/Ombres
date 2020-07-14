// Fill out your copyright notice in the Description page of Project Settings.


#include "SkywalkPlatform.h"
#include "TimerManager.h"
#include "ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SkywalkComponent.h"
#include "Scrap.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
ASkywalkPlatform::ASkywalkPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetVisibility(false);

	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetCollisionProfileName(FName(TEXT("BlockAllDynamic")));
	StaticMeshComponent->SetRelativeScale3D(FVector(3,3,3));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve1(TEXT("/Game/BP/Curves/ScrapGrabCurve"));
	check(Curve1.Succeeded());

	FloatCurve1 = Curve1.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve2(TEXT("/Game/BP/Curves/ScrapPlaceCurve"));
	check(Curve2.Succeeded());

	FloatCurve2 = Curve2.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve3(TEXT("/Game/BP/Curves/ScrapRotationCurve"));
	check(Curve3.Succeeded());

	RotationCurve = Curve3.Object;





}


void ASkywalkPlatform::CalculateCommonPositions()
{
	ACharacter* Player = skywalkComponent->Player;
	APlayerCameraManager* CameraManager = skywalkComponent->CameraManager;
	FVector playerPosition = Player->GetActorLocation();
	FVector cameraPosition = CameraManager->GetCameraLocation();
	FRotator cameraRotation = CameraManager->GetCameraRotation();

	FVector cameraForwardVector = skywalkComponent->TargetRotation.Vector();
	FVector cameraRightVector = (CameraManager->GetCameraRotation() + FRotator(0, 90, 0)).Vector() * (skywalkComponent->ScrapsPerLine % 2 == 0 ? skywalkComponent->SpaceBetweenScraps / 2 : skywalkComponent->SpaceBetweenScraps);
	FVector offsetVector = FVector(0, 0, -25);

	FVector platformLocation = GetActorLocation();
	FVector platformLocationALittleAbove = platformLocation + FVector(0, 0, 100);

	ScrapFinalMiddlePosition2 = platformLocation + cameraForwardVector * (200) + offsetVector - cameraRightVector;
	ScrapFinalMiddlePosition = platformLocation + offsetVector - cameraRightVector;
	ScrapMiddlePosition2 = cameraPosition + cameraForwardVector * (skywalkComponent->DistanceFromCamera2) - cameraRightVector;
	ScrapMiddlePosition = cameraPosition + cameraForwardVector * (skywalkComponent->DistanceFromCamera) - cameraRightVector;
	ScrapRightOffset = (CameraManager->GetCameraRotation() + FRotator(0, 90, 0)).Vector() * skywalkComponent->SpaceBetweenScraps;
}

void ASkywalkPlatform::SetPaused(bool paused)
{
	for (int i = 0; i < activeScraps1.Num(); i++) {
		if (IsValid(activeScraps1[i])) {
			activeScraps1[i]->CustomTimeDilation = paused ? 0 : 1;
		}
		else {
			activeScraps1.RemoveAt(i);
			i--;
			continue;
		}
	}
	for (int i = 0; i < activeScraps2.Num(); i++) {
		if (IsValid(activeScraps2[i])) {
			activeScraps2[i]->CustomTimeDilation = paused ? 0 : 1;
		}
		else {
			activeScraps2.RemoveAt(i);
			i--;
			continue;
		}
	}
	if (paused) {
		GetWorldTimerManager().PauseTimer(timerHandle);
		GetWorldTimerManager().PauseTimer(timerHandle2);
		GetWorldTimerManager().PauseTimer(secondLineTimerHandle);
		GetWorldTimerManager().PauseTimer(firstLineTimerHandle);
	}
	else {
		GetWorldTimerManager().UnPauseTimer(timerHandle);
		GetWorldTimerManager().UnPauseTimer(timerHandle2);
		GetWorldTimerManager().UnPauseTimer(secondLineTimerHandle);
		GetWorldTimerManager().UnPauseTimer(firstLineTimerHandle);
	}
	

	SetActorTickEnabled(!paused);
}

void ASkywalkPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}



//add an entry to all arrays
void ASkywalkPlatform::AddScrap(AActor* scrapToAdd, int LineIndex, int Line)
{
	skywalkComponent->ScrapsInUse.Add(scrapToAdd);
	if (Line == 0) {
		activeScraps1[LineIndex] = scrapToAdd;
		firstLineNoiseRefPositions[LineIndex] = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(0, 0, -1), 45) * NoiseAmplitude;
		firstLineBigNoiseRefPositions[LineIndex] = UKismetMathLibrary::RandomUnitVector() * NoiseAmplitude * 3;
		startPositions1[LineIndex] = scrapToAdd->GetActorLocation();
		startRotations1[LineIndex] = scrapToAdd->GetActorRotation();

		if (EnablePreviews) {
			AStaticMeshActor* preview = GetWorld()->SpawnActor<AStaticMeshActor>();
			preview->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			preview->GetStaticMeshComponent()->SetStaticMesh(Cast<UStaticMeshComponent>(scrapToAdd->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetStaticMesh());
			preview->GetStaticMeshComponent()->SetMaterial(0, skywalkComponent->PreviewMaterial);
			preview->SetActorEnableCollision(false);
			previews1[LineIndex] = preview;
		}

	}
	else {
		activeScraps2[LineIndex] = scrapToAdd;
		secondLineNoiseRefPositions[LineIndex] = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(0, 0, -1), 45) * NoiseAmplitude;
		secondLineBigNoiseRefPositions[LineIndex] = UKismetMathLibrary::RandomUnitVector() * NoiseAmplitude * 3;
		startPositions2[LineIndex] = scrapToAdd->GetActorLocation();
		startRotations2[LineIndex] = scrapToAdd->GetActorRotation();
		if (EnablePreviews) {
			AStaticMeshActor* preview = GetWorld()->SpawnActor<AStaticMeshActor>();
			preview->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			preview->GetStaticMeshComponent()->SetStaticMesh(Cast<UStaticMeshComponent>(scrapToAdd->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetStaticMesh());
			preview->GetStaticMeshComponent()->SetMaterial(0, skywalkComponent->PreviewMaterial);
			preview->SetActorEnableCollision(false);
			previews2[LineIndex] = preview;
		}
	}
}


void ASkywalkPlatform::Tick(float DeltaTime) {

	FVector targetPosition;

	CalculateCommonPositions();
	if (activeScraps1.Num() != 0) {

		progression1 += DeltaTime;
		float percent;

		if (progression1 < BringScrapDuration) {
			percent = progression1 / BringScrapDuration;
			for (int i = 0, l = activeScraps1.Num(); i < l; i++) {
				targetPosition = CalculateScrapTargetPosition(i, 0);
				activeScraps1[i]->SetActorLocation(UKismetMathLibrary::VLerp(startPositions1[i], targetPosition, FloatCurve1->GetFloatValue(percent)));
				activeScraps1[i]->SetActorRotation(UKismetMathLibrary::RLerp(startRotations1[i], skywalkComponent->TargetRotation, RotationCurve->GetFloatValue(progression1 / (BringScrapDuration + PlaceScrapDuration)), true));

				if (EnablePreviews) {
					previews1[i]->SetActorLocation(CalculateScrapFinalPosition(i, 0));
					previews1[i]->SetActorRotation(skywalkComponent->TargetRotation);
				}
			}

		}
		else {
			percent = (progression1 - BringScrapDuration) / PlaceScrapDuration;
			if (percent < 1) {
				for (int i = 0, l = activeScraps1.Num(); i < l; i++) {
					targetPosition = CalculateScrapFinalPosition(i, 0);
					activeScraps1[i]->SetActorLocation(UKismetMathLibrary::VLerp(intermediatePositions1[i], targetPosition, FloatCurve2->GetFloatValue(percent)));
					activeScraps1[i]->SetActorRotation(UKismetMathLibrary::RLerp(startRotations1[i], skywalkComponent->TargetRotation, RotationCurve->GetFloatValue(progression1 / (BringScrapDuration + PlaceScrapDuration)), true));
					if (EnablePreviews) {
						previews1[i]->SetActorLocation(targetPosition);
						previews1[i]->SetActorRotation(skywalkComponent->TargetRotation);
					}
				}
			}
			else {
				GetWorldTimerManager().SetTimer(firstLineTimerHandle, this, &ASkywalkPlatform::ResetPhysics1, ScrapLifeTime, false);
				levitatingScraps1 = TArray<AActor*>(activeScraps1);
				activeScraps1.Empty();
				startPositions1.Empty();
				intermediatePositions1.Empty();
				firstLineBigNoiseRefPositions.Empty();
				firstLineNoiseRefPositions.Empty();
				startRotations1.Empty();
				if (EnablePreviews) {
					for (int j = 0, ll = previews1.Num(); j < ll; j++) {
						previews1[j]->Destroy();
					}
					previews1.Empty();
				}
			}
		}
	}

	if (activeScraps2.Num() != 0) {
		progression2 += DeltaTime;
		float percent;

		if (progression2 < BringScrapDuration) {
			percent = progression2 / BringScrapDuration;
			for (int i = 0, l = activeScraps2.Num(); i < l; i++) {
				targetPosition = CalculateScrapTargetPosition(i, 1);
				activeScraps2[i]->SetActorLocation(UKismetMathLibrary::VLerp(startPositions2[i], targetPosition, FloatCurve1->GetFloatValue(percent)));
				activeScraps2[i]->SetActorRotation(UKismetMathLibrary::RLerp(startRotations2[i], skywalkComponent->TargetRotation, RotationCurve->GetFloatValue(progression2 / (BringScrapDuration + PlaceScrapDuration)), true));
				
				if (EnablePreviews) {
					previews2[i]->SetActorLocation(CalculateScrapFinalPosition(i, 1));
					previews2[i]->SetActorRotation(skywalkComponent->TargetRotation);
				}
			}
			
		}
		else {
			percent = (progression1 - BringScrapDuration) / PlaceScrapDuration;
			if (percent < 1) {
				for (int i = 0, l = activeScraps2.Num(); i < l; i++) {
					targetPosition = CalculateScrapFinalPosition(i, 1);
					activeScraps2[i]->SetActorLocation(UKismetMathLibrary::VLerp(intermediatePositions2[i], CalculateScrapFinalPosition(i, 1), FloatCurve2->GetFloatValue(percent)));
					activeScraps2[i]->SetActorRotation(UKismetMathLibrary::RLerp(startRotations2[i], skywalkComponent->TargetRotation, RotationCurve->GetFloatValue(progression2 / (BringScrapDuration + PlaceScrapDuration)), true));
					
					if (EnablePreviews) {
						previews2[i]->SetActorLocation(targetPosition);
						previews2[i]->SetActorRotation(skywalkComponent->TargetRotation);
					}
				}


			}
			else {
				GetWorldTimerManager().SetTimer(secondLineTimerHandle, this, &ASkywalkPlatform::ResetPhysics2, ScrapLifeTime, false);
				levitatingScraps2 = TArray<AActor*>(activeScraps2);
				activeScraps2.Empty();
				startPositions2.Empty();
				intermediatePositions2.Empty();
				secondLineBigNoiseRefPositions.Empty();
				secondLineNoiseRefPositions.Empty();
				startRotations2.Empty();
				if (EnablePreviews){
					for (int j = 0, ll = previews2.Num(); j < ll; j++) {
						previews2[j]->Destroy();
					}
					previews2.Empty();
				}
				
				SetActorTickEnabled(false);
			}
		}
	}
	
	
}


void ASkywalkPlatform::ResetPhysics1()
{
	for (int i = 0, l = levitatingScraps1.Num(); i < l; i++) {
		if (IsValid(levitatingScraps1[i])) {
			UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(levitatingScraps1[i]->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			smc->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			smc->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel18, ECollisionResponse::ECR_Ignore);
			smc->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
			smc->SetSimulatePhysics(true);
			smc->SetEnableGravity(true);
		}
		
	}
	

}


void ASkywalkPlatform::ResetPhysics2()
{
	for (int i = 0, l = levitatingScraps2.Num(); i < l; i++) {
		if (IsValid(levitatingScraps2[i])) {
			UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(levitatingScraps2[i]->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			smc->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			smc->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel18, ECollisionResponse::ECR_Ignore);
			smc->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
			smc->SetSimulatePhysics(true);
			smc->SetEnableGravity(true);
		}
	}
	
}
FVector ASkywalkPlatform::CalculateScrapTargetPosition(int scrapIndex,int line)
{
	FVector BigNoiseRefPosition;
	FVector middlePosition;

	if (line == 0) {
		middlePosition = ScrapMiddlePosition;
		BigNoiseRefPosition = firstLineBigNoiseRefPositions[scrapIndex];
		
	}
	else {
		middlePosition = ScrapMiddlePosition2;
		BigNoiseRefPosition = secondLineBigNoiseRefPositions[scrapIndex];
	}
	FVector res = middlePosition + BigNoiseRefPosition + ScrapRightOffset * scrapIndex;

	if (line == 0) {
		intermediatePositions1[scrapIndex] = res;
	}
	else {
		intermediatePositions2[scrapIndex] = res;
	}

	//UE_LOG(LogTemp, Warning, TEXT("scrap target position : %s"), *res.ToString());
	return res;
}

FVector ASkywalkPlatform::CalculateScrapFinalPosition(int scrapIndex,int line)
{
	FVector NoiseRefPosition;
	FVector middlePosition;

	if (line == 0) {
		middlePosition = ScrapFinalMiddlePosition;
		NoiseRefPosition = firstLineNoiseRefPositions[scrapIndex];
	}
	else {
		middlePosition = ScrapFinalMiddlePosition2;
		NoiseRefPosition = secondLineNoiseRefPositions[scrapIndex];
	}

	return middlePosition + NoiseRefPosition + ScrapRightOffset * scrapIndex;
}



void ASkywalkPlatform::Init() {
	tiles = skywalkComponent->TilesMeshes;
	signs = skywalkComponent->SignsMeshes;

	GetWorldTimerManager().SetTimer(timerHandle, this, &ASkywalkPlatform::DestroyActor, FadeTime+0.1);
	EnablePreviews = skywalkComponent->EnablePreviews;
}


void ASkywalkPlatform::DestroyActor()
{
	levitatingScraps1.Append(activeScraps1);
	levitatingScraps2.Append(activeScraps2);
	ResetPhysics1();
	ResetPhysics2();
	GetWorldTimerManager().ClearTimer(timerHandle);
	GetWorldTimerManager().ClearTimer(firstLineTimerHandle);
	GetWorldTimerManager().ClearTimer(secondLineTimerHandle);
	Destroy();
}


void ASkywalkPlatform::SetDelay(float value)
{
	GetWorld()->GetTimerManager().SetTimer(timerHandle2, this, &ASkywalkPlatform::SpawnSecondLine, value, false);
}


void ASkywalkPlatform::SpawnFirstLine()
{
	int arraySize = skywalkComponent->ScrapsPerLine;

	//set array size 
	activeScraps1.SetNum(arraySize);
	startPositions1.SetNum(arraySize);
	firstLineBigNoiseRefPositions.SetNum(arraySize);
	firstLineNoiseRefPositions.SetNum(arraySize);
	intermediatePositions1.SetNum(arraySize);
	startRotations1.SetNum(arraySize);
	if (EnablePreviews) {
		previews1.SetNum(arraySize);
	}

	progression1 = 0;

	for (int i = 0, l = arraySize; i < l; i++) {
		MoveClosestScrap(i, 0);
	}
}


void ASkywalkPlatform::SpawnSecondLine() {
	int arraySize = skywalkComponent->ScrapsPerLine;

	//set array size 
	activeScraps2.SetNum(arraySize);
	startPositions2.SetNum(arraySize);
	secondLineBigNoiseRefPositions.SetNum(arraySize);
	secondLineNoiseRefPositions.SetNum(arraySize);
	intermediatePositions2.SetNum(arraySize);
	progression2 = 0;
	startRotations2.SetNum(arraySize);
	if (EnablePreviews) {
		previews2.SetNum(arraySize);
	}

	for (int i = 0, l = arraySize; i < l; i++) {
		MoveClosestScrap(i, 1);
	}
}


void ASkywalkPlatform::MoveClosestScrap(int LineIndex, int Line)
{
	AActor* NearestScrap = skywalkComponent->GetClosestScrap();
	//si un scrap est dispo
	if (IsValid(NearestScrap)) {
		UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(NearestScrap->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (IsValid(staticMeshComponent)) {
			staticMeshComponent->SetMobility(EComponentMobility::Movable);
			staticMeshComponent->SetSimulatePhysics(false);
			if (staticMeshComponent!=NearestScrap->GetRootComponent() && !staticMeshComponent->IsAttachedTo(NearestScrap->GetRootComponent())) {
				staticMeshComponent->AttachToComponent(NearestScrap->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
			}
			staticMeshComponent->SetEnableGravity(false);
			staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			skywalkComponent->ScrapsInWorld.Remove(NearestScrap);
		}
	}
	//sinon, on en spawn un de manière random
	else {
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FVector cameraPosition = cameraManager->GetCameraLocation();
		FVector cameraRightVector = cameraManager->GetCameraRotation().Vector().RightVector;
		FVector pos = cameraPosition + (cameraRightVector * (FMath::Rand() / RAND_MAX * 2 - 1) * SpawnDistance);

		FVector targetPosition = FVector(pos.X, pos.Y, skywalkComponent->Player->GetActorLocation().Z - 150);
		FRotator targetRotation = FRotator(FMath::FRandRange(0, 180), FMath::FRandRange(0, 180), FMath::FRandRange(0, 180));

		NearestScrap = GetWorld()->SpawnActor(AStaticMeshActor::StaticClass(), &targetPosition, &targetRotation);
		AStaticMeshActor* staticMeshActor = (Cast<AStaticMeshActor>(NearestScrap));
		staticMeshActor->SetMobility(EComponentMobility::Movable);
		UStaticMeshComponent* meshComp = staticMeshActor->GetStaticMeshComponent();

		meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		meshComp->SetCollisionProfileName(FName(TEXT("Custom")));
		meshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel18, ECollisionResponse::ECR_Ignore);
		meshComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel18);

		float rand = FMath::FRand();

		if (rand<=skywalkComponent->TilesSpawnProbability){
			//80% de chances de faire pop des tuiles
			meshComp->SetStaticMesh(tiles[FMath::RandRange(0, tiles.Num() - 1)]);
		}
		else {
			meshComp->SetStaticMesh(signs[FMath::RandRange(0,signs.Num()-1)]);
		}

		

		//UE_LOG(LogTemp, Warning, TEXT("object type : %s"),*GETENUMSTRING("ECollisionChannel", meshComp->GetCollisionObjectType()));
	}
	AddScrap(NearestScrap, LineIndex, Line);
}
