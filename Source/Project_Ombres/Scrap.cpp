// Fill out your copyright notice in the Description page of Project Settings.


#include "Scrap.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "ConstructorHelpers.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"

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
	StaticMeshComponent->SetCollisionProfileName(FName(TEXT("Custom")));
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);


	StaticMeshComponent->SetGenerateOverlapEvents(false);

	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve1(TEXT("/Game/BP/Curves/ScrapGrabCurve"));
	check(Curve1.Succeeded());

	FloatCurve1 = Curve1.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve2(TEXT("/Game/BP/Curves/ScrapPlaceCurve"));
	check(Curve2.Succeeded());

	FloatCurve2 = Curve2.Object;

	SetActorTickEnabled(false);
	

}


void AScrap::BeginPlay()
{
	Super::BeginPlay();
	//StaticMeshComponent->SetWorldScale3D(FVector(FMath::FRandRange(0.2,0.8), FMath::FRandRange(0.2, 0.8), FMath::FRandRange(0.2, 0.8)));
	float rand = FMath::FRandRange(0.2, 0.8);
	StaticMeshComponent->SetWorldScale3D(FVector(rand, rand, rand));
}


void AScrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (grabbing) {
		timeElapsed += DeltaTime;
		float percent = timeElapsed / bringDuration;
		if (percent < 1) {
			Timeline1UpdateCallback(FloatCurve1->GetFloatValue(percent));
		}
		else {
			Timeline1FinishedCallback(1);
		}
	}
	else if (placing) {
		timeElapsed += DeltaTime;
		float percent = timeElapsed / placeDuration;
		if (percent < 1) {
			Timeline2UpdateCallback(FloatCurve2->GetFloatValue(percent));
		}
		else {
			Timeline2FinishedCallback(1);
		}
	}

}

void AScrap::StartMovingToTarget(float NoiseAmplitude, ACharacter* Player, float ScrapBringDuration, float ScrapPlaceDuration, int LineIndex, int Line,float LifeTime)
{
	UE_LOG(LogTemp, Warning, TEXT("LineIndex : %d, Line : %d"), LineIndex, Line);
	this->Player = Player;
	this->LineIndex = LineIndex;
	this->Line = Line;
	this->LifeTime = LifeTime;

	Skywalk = Cast<USkywalkComponent>(Player->GetComponentByClass(USkywalkComponent::StaticClass()));
	StartPosition1 = GetActorLocation();
	NoiseRefPosition = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(0, 0, -1), 45) * NoiseAmplitude;
	BigNoiseRefPosition = UKismetMathLibrary::RandomUnitVector() * NoiseAmplitude * 3;
	bringDuration = ScrapBringDuration;
	placeDuration = ScrapPlaceDuration;

	grabbing = true;
	timeElapsed = 0;
	SetActorTickEnabled(true);
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
	grabbing = false;
	placing = true;
	timeElapsed = 0;
}



void AScrap::Timeline2UpdateCallback(float val)
{
	SetActorLocation(UKismetMathLibrary::VLerp(StartPosition2, CalculateScrapFinalPosition((Line == 0 ? Skywalk->ScrapFinalMiddlePosition : Skywalk->ScrapFinalMiddlePosition2), Skywalk->ScrapRightOffset, LineIndex), val));
}



void AScrap::Timeline2FinishedCallback(float val)
{
	placing = false;
	GetWorldTimerManager().SetTimer(timerHandle, this, &AScrap::ResetPhysics,LifeTime, false);
	SetActorTickEnabled(false);

}


void AScrap::ResetPhysics()
{
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetEnableGravity(true);
}
