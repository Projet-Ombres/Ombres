// Fill out your copyright notice in the Description page of Project Settings.


#include "Scrap.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "ConstructorHelpers.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AScrap::AScrap()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"), false);
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"), false);
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);


	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("StaticMesh'/Engine/BasicShapes/BasicShapeMaterial'"));
	StaticMeshComponent->SetMaterial(0, Material.Object);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StaticMeshComponent->SetGenerateOverlapEvents(false);

	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve1(TEXT("/Game/BP/Curves/ScrapGrabCurve"));
	check(Curve1.Succeeded());

	FloatCurve1 = Curve1.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve2(TEXT("/Game/BP/Curves/ScrapPlaceCurve"));
	check(Curve2.Succeeded());

	FloatCurve2 = Curve2.Object;
	

}



// Called when the game starts or when spawned
void AScrap::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->SetWorldScale3D(FVector(FMath::FRandRange(0.2,0.8), FMath::FRandRange(0.2, 0.8), FMath::FRandRange(0.2, 0.8)));

	FOnTimelineFloat onTimelineUpdateCallback1;
	FOnTimelineEventStatic onTimelineFinishedCallback1;

	if (FloatCurve1 != NULL) {
		Timeline1 = NewObject<UTimelineComponent>(this, FName("Timeline1"));
		Timeline1->CreationMethod = EComponentCreationMethod::UserConstructionScript;

		Timeline1->SetPropertySetObject(this);
		Timeline1->SetDirectionPropertyName(FName("TimelineDirection1"));

		Timeline1->SetLooping(false);
		Timeline1->SetTimelineLength(1);
		Timeline1->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);

		Timeline1->SetPlaybackPosition(0.0f, false);

		onTimelineUpdateCallback1.BindUFunction(this, FName{ TEXT("Timeline1UpdateCallback") });
		onTimelineFinishedCallback1.BindUFunction(this, FName{ TEXT("Timeline1FinishedCallback") });
		Timeline1->AddInterpFloat(FloatCurve1, onTimelineUpdateCallback1);
		Timeline1->SetTimelineFinishedFunc(onTimelineFinishedCallback1);

		Timeline1->RegisterComponent();
	}

	FOnTimelineFloat onTimelineUpdateCallback2;
	FOnTimelineEventStatic onTimelineFinishedCallback2;

	if (FloatCurve2 != NULL) {


		Timeline2 = NewObject<UTimelineComponent>(this, FName("Timeline2"));


		Timeline2->SetLooping(false);
		Timeline2->SetTimelineLength(1);

		onTimelineUpdateCallback2.BindUFunction(this, FName("Timeline2UpdateCallback"));
		onTimelineFinishedCallback2.BindUFunction(this, FName("Timeline2FinishedCallback"));
		Timeline2->AddInterpFloat(FloatCurve2, onTimelineUpdateCallback2);
		Timeline2->SetTimelineFinishedFunc(onTimelineFinishedCallback2);

		Timeline2->RegisterComponent();
	}


	
}

// Called every frame
void AScrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Timeline1 != NULL) {
		Timeline1->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
		
	}

	if (Timeline2 != NULL) {
		Timeline2->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}

}

void AScrap::StartMovingToTarget(float NoiseAmplitude, ACharacter* Player, float ScrapBringDuration, float ScrapPlaceDuration, int LineIndex, int Line,float LifeTime)
{
	this->Player = Player;
	this->LineIndex = LineIndex;
	this->Line = Line;
	this->LifeTime = LifeTime;

	Skywalk = Cast<USkywalkComponent>(Player->GetComponentByClass(USkywalkComponent::StaticClass()));
	StartPosition1 = GetActorLocation();
	NoiseRefPosition = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(0, 0, -1), 45) * NoiseAmplitude;
	BigNoiseRefPosition = UKismetMathLibrary::RandomUnitVector() * NoiseAmplitude * 3;

	Timeline1->PlayFromStart();
}

FVector AScrap::CalculateScrapTargetPosition(FVector middlePosition, FVector rightOffset, int lineIndex)
{
	return middlePosition + BigNoiseRefPosition + rightOffset * lineIndex;
}

FVector AScrap::CalculateScrapFinalPosition(FVector middlePosition, FVector rightOffset, int index)
{
	return middlePosition + NoiseRefPosition + rightOffset * index;
}




void AScrap::Timeline1UpdateCallback(float val)
{
	SetActorLocation(UKismetMathLibrary::VLerp(StartPosition1, CalculateScrapTargetPosition((Line == 0 ? Skywalk->ScrapMiddlePosition : Skywalk->ScrapMiddlePosition2), Skywalk->ScrapRightOffset, LineIndex),val));
}

void AScrap::Timeline1FinishedCallback(float val)
{
	StartPosition2 = GetActorLocation();
	Timeline2->PlayFromStart();
}



void AScrap::Timeline2UpdateCallback(float val)
{
	SetActorLocation(UKismetMathLibrary::VLerp(StartPosition2, CalculateScrapFinalPosition((Line == 0 ? Skywalk->ScrapFinalMiddlePosition : Skywalk->ScrapFinalMiddlePosition2), Skywalk->ScrapRightOffset, LineIndex), val));
}



void AScrap::Timeline2FinishedCallback(float val)
{
	GetWorldTimerManager().SetTimer(timerHandle, this, &AScrap::ResetPhysics, 1 / LifeTime, false);
}


void AScrap::ResetPhysics()
{
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetEnableGravity(true);
}
