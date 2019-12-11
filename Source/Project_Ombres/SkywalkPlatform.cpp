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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Prop1(TEXT("/Game/_ART/StaticMesh/SM_Large_Signboard"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Prop2(TEXT("/Game/_ART/StaticMesh/SM_Midel_Signboard"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Prop3(TEXT("/Game/_ART/StaticMesh/SM_Small_Signboard"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Prop4(TEXT("/Game/_ART/StaticMesh/SM_Wall_Light_1"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Prop5(TEXT("/Game/_ART/StaticMesh/SM_Wall_Light_2"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Prop6(TEXT("/Game/_ART/StaticMesh/SM_RoofSlides_SM_RoofSlides_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Prop7(TEXT("/Game/_ART/StaticMesh/SM_RoofSlides_SM_RoofSlides_02"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Prop8(TEXT("/Game/_ART/StaticMesh/SM_RoofSlides_SM_RoofSlides_03"));


	props.SetNum(8);
	props[0]=Prop1.Object;
	props[1]=Prop2.Object;
	props[2]=Prop3.Object;
	props[3]=Prop4.Object;
	props[4]=Prop5.Object;
	props[5] = Prop6.Object;
	props[6] = Prop7.Object;
	props[7] = Prop8.Object;
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
	}
	else {
		activeScraps2[LineIndex] = scrapToAdd;
		secondLineNoiseRefPositions[LineIndex] = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(0, 0, -1), 45) * NoiseAmplitude;
		secondLineBigNoiseRefPositions[LineIndex] = UKismetMathLibrary::RandomUnitVector() * NoiseAmplitude * 3;
		startPositions2[LineIndex] = scrapToAdd->GetActorLocation();
		startRotations2[LineIndex] = scrapToAdd->GetActorRotation();
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
				activeScraps1[i]->SetActorRotation(UKismetMathLibrary::RLerp(startRotations1[i], skywalkComponent->TargetRotation, RotationCurve->GetFloatValue(progression1/(BringScrapDuration+PlaceScrapDuration)),true));
			}
		}
		else {
			percent = (progression1 - BringScrapDuration) / PlaceScrapDuration;
			if (percent < 1) {
				for (int i = 0, l = activeScraps1.Num(); i < l; i++) {
					activeScraps1[i]->SetActorLocation(UKismetMathLibrary::VLerp(intermediatePositions1[i], CalculateScrapFinalPosition(i, 0), FloatCurve2->GetFloatValue(percent)));
					activeScraps1[i]->SetActorRotation(UKismetMathLibrary::RLerp(startRotations1[i], skywalkComponent->TargetRotation, RotationCurve->GetFloatValue(progression1 / (BringScrapDuration + PlaceScrapDuration)), true));

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
				activeScraps2[i]->SetActorRotation(UKismetMathLibrary::RLerp(startRotations2[i], skywalkComponent->TargetRotation, RotationCurve->GetFloatValue(progression2 / (BringScrapDuration + PlaceScrapDuration)), true));
			}
		}
		else {
			percent = (progression1 - BringScrapDuration) / PlaceScrapDuration;
			if (percent < 1) {
				for (int i = 0, l = activeScraps1.Num(); i < l; i++) {
					activeScraps2[i]->SetActorLocation(UKismetMathLibrary::VLerp(intermediatePositions2[i], CalculateScrapFinalPosition(i, 1), FloatCurve2->GetFloatValue(percent)));
					activeScraps2[i]->SetActorRotation(UKismetMathLibrary::RLerp(startRotations2[i], skywalkComponent->TargetRotation, RotationCurve->GetFloatValue(progression2 / (BringScrapDuration + PlaceScrapDuration)), true));
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
		smc->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel18, ECollisionResponse::ECR_Ignore);
		smc->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		smc->SetSimulatePhysics(true);
		smc->SetEnableGravity(true);
	}
	

}


void ASkywalkPlatform::ResetPhysics2()
{
	for (int i = 0, l = levitatingScraps2.Num(); i < l; i++) {
		UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(levitatingScraps2[i]->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		smc->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		smc->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel18, ECollisionResponse::ECR_Ignore);
		smc->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
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
	startRotations1.SetNum(arraySize);

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
		meshComp->SetStaticMesh(props[FMath::RandRange(0,props.Num() - 1)]);

		//UE_LOG(LogTemp, Warning, TEXT("object type : %s"),*GETENUMSTRING("ECollisionChannel", meshComp->GetCollisionObjectType()));
	}
	AddScrap(NearestScrap, LineIndex, Line);
}
