// Fill out your copyright notice in the Description page of Project Settings.


#include "WallrunComponent.h"
#include "GameFramework/Character.h"
#include "Curves/CurveFloat.h"
#include "Camera/CameraShake.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "ConstructorHelpers.h"

UWallrunComponent::UWallrunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Duration = 0.92f;
	CurveMaxHeight = 450;
	CameraRollAngle = 15;
	SnapToWallDuration = 0.2f;
	SnapDistance = 150;
	WallJumpForwardForce = 573;
	WallJumpNormalForce = 250;
	WallJumpUpForce = 720;
	MaxAngleToWall = 90;
	stopWallrunTolerance = 0.2f;
	CameraRollTransitionDuration = 0.2f;
	bIsDoingSomethingElse = false;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> heightCurve(TEXT("CurveFloat'/Game/BP/Curves/WallRunCurve.WallRunCurve'"));
	if (heightCurve.Succeeded()) {
		HeightCurve = heightCurve.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> speedCurve(TEXT("CurveFloat'/Game/BP/Curves/WallRunSpeed.WallRunSpeed'"));
	if (speedCurve.Succeeded()) {
		SpeedCurve = speedCurve.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> rollCurve(TEXT("CurveFloat'/Game/BP/Curves/CameraRollCurve.CameraRollCurve'"));
	if (rollCurve.Succeeded()) {
		CameraRollCurve = rollCurve.Object;
	}

	static ConstructorHelpers::FObjectFinder<UClass> cameraShake(TEXT("Blueprint'/Game/BP/FeedbackAssets/ScreenShakes/BP_CameraShake_Wallrun.BP_CameraShake_Wallrun_C'"));
	if (cameraShake.Succeeded()) {
		ScreenShake = cameraShake.Object;
	}
}


// Called when the game starts
void UWallrunComponent::BeginPlay()
{
	Super::BeginPlay();

	playerCharacter = Cast<ACharacter>(GetOwner());

	if (!IsValid(HeightCurve) || !IsValid(SpeedCurve) || !IsValid(CameraRollCurve)) {
		UE_LOG(LogTemp, Error, TEXT("Missing curve in Wallrun. Destroying component..."));
		DestroyComponent();
	}

	
}


// Called every frame
void UWallrunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	WallrunTracer();

	if (bIsWallrunning) {
		wallrunCurrentTime += DeltaTime;
		wallrunLocalCurrentTime += DeltaTime;

		//ici on prend le temps total de wall run sans avoir touché le sol, on ne reset pas la curve
		if (wallrunCurrentTime < Duration) {
			UpdateWallrunCurve(wallrunCurrentTime / Duration);


		}
		else {
			if (bNeedToStopCurve) {
				EndWallrun();
			}
		}

		//ici on prend le temps "local", qui est reset à chaque wallrun consécutif
		if (wallrunLocalCurrentTime< CameraRollTransitionDuration) {
			UpdateCameraRoll(wallrunLocalCurrentTime / CameraRollTransitionDuration);
			
		}
		else {
			if (bNeedToStopCameraUpdates) {
				SetCameraRoll(1);
				bNeedToStopCameraUpdates = false;
			}
		}

		
	}
	else if (bStoppingWallrun) {
		cameraRollEndTime += DeltaTime;

		if (cameraRollEndTime < CameraRollTransitionDuration) {
			UpdateCameraRoll(1 - (cameraRollEndTime / CameraRollTransitionDuration));
		}
		else {
			bStoppingWallrun = false;
			cameraRollEndTime = 0;
			SetCameraRoll(0);
		}
	}




}

void UWallrunComponent::WallrunTracer()
{
	FHitResult hitResult;
	FVector playerHipsLocation = playerCharacter->GetActorLocation() + FVector(0, 0, 50);
	FVector start = playerCharacter->GetActorForwardVector() * (-50) + playerHipsLocation;
	FVector end = playerCharacter->GetActorRightVector() * (-SnapDistance) + playerHipsLocation + playerCharacter->GetActorForwardVector() * (-10);
	
	FCollisionQueryParams queryParam;


	bool tracer1touched = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_GameTraceChannel3,queryParam);
	FVector normal1 = hitResult.ImpactNormal;


	start = playerHipsLocation + playerCharacter->GetActorForwardVector() * 50;
	end = playerHipsLocation + playerCharacter->GetActorRightVector() * (-SnapDistance) + playerCharacter->GetActorForwardVector() * 10;
	
	bool tracer2touched = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_GameTraceChannel3);
	FVector normal2 = hitResult.ImpactNormal;

	bInFrontOfWall = tracer1touched && tracer2touched && (UKismetMathLibrary::EqualEqual_VectorVector(normal1, normal2, 0.001f));


	bool left=false;

	if (bInFrontOfWall) {
		left = true;
	}else{
		start= playerHipsLocation + playerCharacter->GetActorForwardVector() * (-50);
		end = playerHipsLocation + playerCharacter->GetActorRightVector() * SnapDistance + playerCharacter->GetActorForwardVector() * (-10);
		tracer1touched = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_GameTraceChannel3);
		normal1 = hitResult.ImpactNormal;

		start = playerHipsLocation + playerCharacter->GetActorForwardVector() * (50);
		end = playerHipsLocation + playerCharacter->GetActorRightVector() * SnapDistance + playerCharacter->GetActorForwardVector() * (10);
		tracer2touched = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_GameTraceChannel3);
		normal2 = hitResult.ImpactNormal;

		bInFrontOfWall = tracer1touched && tracer2touched && (UKismetMathLibrary::EqualEqual_VectorVector(normal1, normal2, 0.001f));

		if (bInFrontOfWall) {
			left = false;
		}
	}

	float angle;
	
	if (bInFrontOfWall) {
		hitLocation = hitResult.ImpactPoint;

		if (hitResult.Actor.Get() != lastRunnedWall) {
			distanceFromWall = UKismetMathLibrary::Distance2D((FVector2D)hitResult.ImpactPoint, (FVector2D)playerCharacter->GetActorLocation());
			bWallrunLeft = left;

			angle = (bWallrunLeft ? -90 : 90);

			wallNormal = hitResult.ImpactNormal;
			wallDirection = wallNormal.RotateAngleAxis(angle, FVector(0, 0, 1));

			if (IsAllowedToWallrun()) {
				lastRunnedWall = hitResult.Actor.Get();
				StartWallrun();
			}
		}
		else {
			DetachFromWall();
		}
	}
	else {
		DetachFromWall();
	}
}


void UWallrunComponent::DetachFromWall()
{
	if (bIsWallrunning && !bStoppingWallrun && !bInFrontOfWall) {
		currentTimeStopWallrun = currentTimeStopWallrun + UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		if (currentTimeStopWallrun >= stopWallrunTolerance && !bJumpingOff) {
			StopWallrun();
			bIsWallrunning = false;
			bStoppingWallrun = true;
			currentTimeStopWallrun = 0;
		}
	}
}



void UWallrunComponent::SetCameraRoll(float Value)
{
	FRotator startRotation;
	FRotator targetRotation;
	FRotator controlRotation = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();
	startRotation = FRotator(controlRotation.Pitch, controlRotation.Yaw, 0);
	if (bWallrunLeft) {
		targetRotation = FRotator(controlRotation.Pitch, controlRotation.Yaw, cameraRollStart);
	}
	else {
		targetRotation = FRotator(controlRotation.Pitch, controlRotation.Yaw, -cameraRollStart);
	}
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(UKismetMathLibrary::RLerp(startRotation, targetRotation, Value,true));
}

void UWallrunComponent::JumpOffWall()
{
	if (!bJumpingOff) {
		FVector cameraForwardVector = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation().Vector();
		playerCharacter->GetCharacterMovement()->Velocity = cameraForwardVector * UKismetMathLibrary::FClamp(FVector::DotProduct(cameraForwardVector, wallDirection) + 0.25, 0, 1) * WallJumpForwardForce + wallNormal * WallJumpNormalForce + FVector::UpVector * WallJumpUpForce;
		
		
		bJumpingOff = true;
		playerCharacter->bUseControllerRotationYaw = true;
		wallrunLocalCurrentTime = 0;
		OnWallJump.Broadcast();
	}
}

FVector UWallrunComponent::CorrectDirectionVector(FVector InVector) const
{
	return FVector();
}

float UWallrunComponent::UnsignAngle(float SignedAngle) const
{
	float resultAngle = SignedAngle;

	if (resultAngle < 0) {
		resultAngle += 360;
	}else if (resultAngle > 360) {
		resultAngle -= 360;
	}
	return resultAngle;
}



bool UWallrunComponent::CompareAngles(float Angle1, float Angle2) const
{
	if (UKismetMathLibrary::Abs(Angle1 - Angle2) < 180) {
		return Angle1 > Angle2;
	}
	else {
		return !(Angle1>Angle2);
	}
}

float UWallrunComponent::CalculateAngleToWall(AActor* WallActor, FVector ImpactPoint) const
{
	FTransform wallTransform = WallActor->GetActorTransform();
	FTransform relativeTransform = UKismetMathLibrary::MakeRelativeTransform(FTransform(wallTransform.GetRotation(),ImpactPoint , wallTransform.GetScale3D()), playerCharacter->GetActorTransform());
	return UKismetMathLibrary::Atan2(relativeTransform.GetLocation().X, relativeTransform.GetLocation().Y);
}

void UWallrunComponent::EndWallrun()
{
	playerCharacter->GetCharacterMovement()->GravityScale = 1;
	bNeedToStopCurve = false;
}

void UWallrunComponent::ResetOnLanded()
{
	lastRunnedWall = nullptr;

	if (bJumpingOff) {
		bJumpingOff = false;
		OnWallrunEnd.Broadcast(wallrunCurrentTime);
	}
	else {
		StopWallrun();
	}

	bIsWallrunning = false;
	playerCharacter->GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
	playerCharacter->bUseControllerRotationYaw = true;
	wallrunCurrentTime = 0;
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopCameraShake(cameraShakeInstance);
}

void UWallrunComponent::StartWallrun()
{
	bJumpingOff = false;
	bNeedToStopCurve = true;
	bNeedToStopCameraUpdates = true;
	wallrunLocalCurrentTime = 0;
	wallrunStartPoint = playerCharacter->GetActorLocation();
	bIsWallrunning = true;
	playerCharacter->GetCharacterMovement()->GravityScale = 0;
	playerCharacter->GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 1));
	cameraRollStart = CameraRollAngle;
	wallrunStartRotation = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();

	FRotator bodyRotation = FVector(wallDirection.X, wallDirection.Y, 0).Rotation();

	playerCharacter->GetCapsuleComponent()->SetWorldRotation(bodyRotation);
	playerCharacter->bUseControllerRotationYaw = false;


	OnWallrunStart.Broadcast();
}

void UWallrunComponent::UpdateWallrunCurve(float Value)
{
	FVector WallForwardVelocity = wallDirection * SpeedCurve->GetFloatValue(Value);
	playerCharacter->GetCharacterMovement()->Velocity = FVector(WallForwardVelocity.X, WallForwardVelocity.Y, HeightCurve->GetFloatValue(Value) * CurveMaxHeight) + wallNormal * (wallrunLocalCurrentTime > SnapToWallDuration ? 0 : (1 / SnapToWallDuration) * (-distanceFromWall));
	cameraShakeInstance = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->PlayCameraShake(ScreenShake);
}

void UWallrunComponent::StopWallrun()
{
	if (!bStoppingWallrun && bIsWallrunning) {
		bStoppingWallrun = true;
		playerCharacter->StopJumping();
		EndWallrun();
		cameraRollStart = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Roll * (bWallrunLeft ? 1 : -1);
		OnWallrunEnd.Broadcast(wallrunCurrentTime);
	}
}

void UWallrunComponent::StopWallrunInstantly() {
	if (bIsWallrunning) {
		EndWallrun();
		SetCameraRoll(0);
		bIsWallrunning = false;
		
	}
}

void UWallrunComponent::UpdateCameraRoll(float Value)
{
	SetCameraRoll(CameraRollCurve->GetFloatValue(Value));
}



void UWallrunComponent::StopCameraRoll()
{
}

bool UWallrunComponent::IsAllowedToWallrun()
{
	return !bIsDoingSomethingElse && playerCharacter->GetCharacterMovement()->IsFalling();
}

