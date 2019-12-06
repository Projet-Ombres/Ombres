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
	}
	else {
		activeScraps2[LineIndex] = scrapToAdd;
		secondLineNoiseRefPositions[LineIndex] = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(0, 0, -1), 45) * NoiseAmplitude;
		secondLineBigNoiseRefPositions[LineIndex] = UKismetMathLibrary::RandomUnitVector() * NoiseAmplitude * 3;
		startPositions2[LineIndex] = scrapToAdd->GetActorLocation();
	}
}


void ASkywalkPlatform::Tick(float DeltaTime){

	if (activeScraps1.Num() != 0) {

		progression1 += DeltaTime;
		float percent;

		if (progression1 < BringScrapDuration) {
			percent = progression1 / BringScrapDuration;
			for (int i = 0, l = activeScraps1.Num(); i < l; i++) {
				activeScraps1[i]->SetActorLocation(UKismetMathLibrary::VLerp(startPositions1[i], CalculateScrapTargetPosition(i, 0),FloatCurve1->GetFloatValue(percent)));
			}
		}
		else {
			percent = (progression1 - BringScrapDuration) / PlaceScrapDuration;
			if (percent < 1) {
				for (int i = 0, l = activeScraps1.Num(); i < l; i++) {
					activeScraps1[i]->SetActorLocation(UKismetMathLibrary::VLerp(intermediatePositions1[i], CalculateScrapFinalPosition(i, 0), FloatCurve2->GetFloatValue(percent)));
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
			}
		}
	}

	if (activeScraps2.Num() != 0) {
		progression2 += DeltaTime;
		float percent;

		if (progression2 < BringScrapDuration) {
			percent = progression2 / BringScrapDuration;
			for (int i = 0, l = activeScraps2.Num(); i < l; i++) {
				activeScraps2[i]->SetActorLocation(UKismetMathLibrary::VLerp(startPositions2[i], CalculateScrapTargetPosition(i, 1), FloatCurve1->GetFloatValue(percent)));
			}
		}
		else {
			percent = (progression1 - BringScrapDuration) / PlaceScrapDuration;
			if (percent < 1) {
				for (int i = 0, l = activeScraps1.Num(); i < l; i++) {
					activeScraps2[i]->SetActorLocation(UKismetMathLibrary::VLerp(intermediatePositions2[i], CalculateScrapFinalPosition(i, 1), FloatCurve2->GetFloatValue(percent)));
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
				SetActorTickEnabled(false);
			}
		}
	}
	
	
}


void ASkywalkPlatform::ResetPhysics1()
{
	for (int i = 0, l = levitatingScraps1.Num(); i < l; i++) {
		UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(levitatingScraps1[i]->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		smc->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		smc->SetSimulatePhysics(true);
		smc->SetEnableGravity(true);
	}
	

}


void ASkywalkPlatform::ResetPhysics2()
{
	for (int i = 0, l = levitatingScraps2.Num(); i < l; i++) {
		UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(levitatingScraps2[i]->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		smc->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		smc->SetSimulatePhysics(true);
		smc->SetEnableGravity(true);
	}
	
}
FVector ASkywalkPlatform::CalculateScrapTargetPosition(int scrapIndex,int line)
{
	FVector rightOffset = skywalkComponent->ScrapRightOffset;
	FVector BigNoiseRefPosition;
	FVector middlePosition;

	if (line == 0) {
		middlePosition = skywalkComponent->ScrapMiddlePosition;
		BigNoiseRefPosition = firstLineBigNoiseRefPositions[scrapIndex];
		
	}
	else {
		middlePosition = skywalkComponent->ScrapMiddlePosition2;
		BigNoiseRefPosition = secondLineBigNoiseRefPositions[scrapIndex];
	}
	FVector res = middlePosition + BigNoiseRefPosition + rightOffset * scrapIndex;

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
	FVector rightOffset = skywalkComponent->ScrapRightOffset;
	FVector NoiseRefPosition;
	FVector middlePosition;

	if (line == 0) {
		middlePosition = skywalkComponent->ScrapFinalMiddlePosition;
		NoiseRefPosition = firstLineNoiseRefPositions[scrapIndex];
	}
	else {
		middlePosition = skywalkComponent->ScrapFinalMiddlePosition2;
		NoiseRefPosition = secondLineNoiseRefPositions[scrapIndex];
	}

	return middlePosition + NoiseRefPosition + rightOffset * scrapIndex;
}



void ASkywalkPlatform::Init() {
	GetWorldTimerManager().SetTimer(timerHandle, this, &ASkywalkPlatform::DestroyActor, FadeTime+0.1);
}


void ASkywalkPlatform::DestroyActor()
{
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

	for (int i = 0, l = arraySize; i < l; i++) {
		MoveClosestScrap(i, 1);
	}
}


void ASkywalkPlatform::MoveClosestScrap(int LineIndex, int Line)
{
	AActor* NearestScrap = skywalkComponent->GetClosestScrap();
	if (IsValid(NearestScrap)) {
		UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(NearestScrap->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (IsValid(staticMeshComponent)) {
			staticMeshComponent->SetSimulatePhysics(false);
			if (staticMeshComponent!=NearestScrap->GetRootComponent() && !staticMeshComponent->IsAttachedTo(NearestScrap->GetRootComponent())) {
				staticMeshComponent->AttachToComponent(NearestScrap->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
			}
			staticMeshComponent->SetEnableGravity(false);
			staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			skywalkComponent->ScrapsInWorld.Remove(NearestScrap);
		}
	}
	else {
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FVector cameraPosition = cameraManager->GetCameraLocation();
		FVector cameraRightVector = cameraManager->GetCameraRotation().Vector().RightVector;
		FVector pos = cameraPosition + (cameraRightVector * (FMath::Rand() / RAND_MAX * 2 - 1) * SpawnDistance);

		FVector targetPosition = FVector(pos.X, pos.Y, 150);

		NearestScrap = GetWorld()->SpawnActor(AScrap::StaticClass(), &targetPosition);

	}
	AddScrap(NearestScrap, LineIndex, Line);
}
