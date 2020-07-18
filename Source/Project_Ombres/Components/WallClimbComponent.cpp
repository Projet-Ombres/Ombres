// Fill out your copyright notice in the Description page of Project Settings.


#include "WallClimbComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UWallClimbComponent::UWallClimbComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CameraAngleAllowed = 80;
	SideTracersOffset = 30;
	EscalateImpulse = 950;
	DecayingSpeedDuration = 0.75f;
	DecayingMaxSpeed = 600;
	CancelTime = 0.2f;
	OneHandCorrectionOffset = 40;
	CameraYawRotationDuration = 0.15f;
	CameraPitchRotationDuration = 0.25f;

	CameraUpPitch = 50;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> upCurve(TEXT("CurveFloat'/Game/BP/Curves/ClimbCameraUp.ClimbCameraUp'"));

	if (upCurve.Succeeded()) {
		CameraUpCurve = upCurve.Object;
	}
}


// Called when the game starts
void UWallClimbComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	wallrunComponent = Cast<UWallrunComponent>(PlayerCharacter->GetComponentByClass(UWallrunComponent::StaticClass()));
	skywalkComponent = Cast<USkywalkComponent>(PlayerCharacter->GetComponentByClass(USkywalkComponent::StaticClass()));
	playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerCamera = Cast<UCameraComponent>(PlayerCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
}


// Called every frame
void UWallClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HeightTracer();


	if (applyingDecayingSpeed) {
		DecayingSpeedTimeLeft -= DeltaTime;

		if (DecayingSpeedTimeLeft > 0) {
			float value = DecayingSpeedTimeLeft / DecayingSpeedDuration;

			FVector forwardVector = FVector(PlayerCharacter->GetActorForwardVector().X, PlayerCharacter->GetActorForwardVector().Y, 0) * DecayingMaxSpeed;
			float dot = FVector::DotProduct(-LastNormal, PlayerCharacter->GetActorForwardVector());
			FVector newPlayerLocation = PlayerCharacter->GetActorLocation() + forwardVector * UKismetMathLibrary::FMax(value, PlayerCharacter->GetCharacterMovement()->GetAnalogInputModifier() / 2) * DeltaTime * (dot > 0 ? dot : 0);


			PlayerCharacter->SetActorLocation(newPlayerLocation);
		}
		else {
			applyingDecayingSpeed = false;
		}
		
	}

	if (bRotatingYaw) {
		CameraYawRotationTimeLeft -= DeltaTime;

		if (CameraYawRotationTimeLeft > 0) {
			FRotator controlRotation = playerController->GetControlRotation();
			float alpha = 1 - (CameraYawRotationTimeLeft / CameraYawRotationDuration);
			playerController->SetControlRotation(UKismetMathLibrary::RLerp(FRotator(controlRotation.Pitch, CameraStartYaw, controlRotation.Roll), FRotator(controlRotation.Pitch, CameraYawTarget, controlRotation.Roll), alpha, true));
		}
		else {
			bRotatingYaw = false;
		}
	}

	if (bRotatingPitch) {
		CameraPitchRotationTimeLeft -= DeltaTime;

		FRotator controlRotation = playerController->GetControlRotation();
		if (CameraPitchRotationTimeLeft > 0) {
			float alpha =CameraUpCurve->GetFloatValue(1 - (CameraPitchRotationTimeLeft / CameraPitchRotationDuration));

			
		
			playerController->SetControlRotation(UKismetMathLibrary::RLerp(FRotator(CameraStartPitch, controlRotation.Yaw, controlRotation.Roll), FRotator(CameraTargetPitch, controlRotation.Yaw, controlRotation.Roll), alpha, true));
		}
		else {
			playerController->ResetIgnoreLookInput();
			playerController->SetControlRotation(FRotator(CameraTargetPitch, controlRotation.Yaw, 0));
			bRotatingPitch = false;
		}
	}



	if (bCancellingClimb) {
		cancelCurrentTime += DeltaTime;
		if (cancelCurrentTime > 0.3f) {
			cancelCurrentTime = 0;
			Drop();
		}
	}
	else {
		cancelCurrentTime = 0;
	}
	
	
}

void UWallClimbComponent::EscalateWall()
{
	HeightToClimb= UKismetMathLibrary::Abs(locationHit.Z - PlayerCharacter->GetActorLocation().Z);
	wallrunComponent->bIsDoingSomethingElse = true;
	bWasReset = false;
	CameraUp(true, 10);
	OnEscalate.Broadcast();
	applyingDecayingSpeed = false;
	DecayingSpeedTimeLeft = 0;
	
	LastNormal = WallNormal;
	PlayerCharacter->GetCharacterMovement()->GravityScale = 1;
	PlayerCharacter->bUseControllerRotationYaw = false;
	ClimbStartHeight = PlayerCharacter->GetActorLocation().Z;
	bIsEscalating = true;

	FVector newPlayerLocation;

	if (wallrunComponent->bIsWallrunning) {
		wallrunComponent->bIsWallrunning = false;
		wallrunComponent->StopWallrun();

		newPlayerLocation = WallNormal * 75 + PlayerCharacter->GetActorLocation();
		PlayerCharacter->SetActorLocation(newPlayerLocation);
	}
	
	skywalkComponent->EndSkyWalk();
	if (IsValid(skywalkComponent->LastPlatformSpawned)) {
		skywalkComponent->LastPlatformSpawned->Destroy();
	}

	PlayerCharacter->GetCharacterMovement()->Velocity = FVector(0, 0, EscalateImpulse);
	RotateCameraYaw();
	newPlayerLocation = PlayerCharacter->GetActorLocation() - WallNormal * 40;
	PlayerCharacter->SetActorLocation(newPlayerLocation);
	
}

void UWallClimbComponent::ClimbLedge()
{
	if (applyingDecayingSpeed) {
		applyingDecayingSpeed = false;
		DecayingSpeedTimeLeft = 0;
		return;
	}
	if (!bIsClimbingLedge && !applyingDecayingSpeed) {
		FRotator controlRotation = playerController->GetControlRotation();
		playerController->SetControlRotation(FRotator(controlRotation.Pitch, (WallNormal * -1).ToOrientationRotator().Yaw, controlRotation.Roll));
		wallrunComponent->StopWallrun();
		wallrunComponent->bIsWallrunning = false;
		PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		PlayerCharacter->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
		bIsEscalating = false;
		bIsClimbingLedge = true;
		CameraUp(false, 2);
		OnLedgeClimb.Broadcast();
	
	}
}

void UWallClimbComponent::CancelClimb()
{
	if (!bCancellingClimb) {
		bCancellingClimb = true;
	}
}

void UWallClimbComponent::StopCancellingClimb()
{
	if (bCancellingClimb) {
		bCancellingClimb = false;
	}
}

void UWallClimbComponent::StopClimbing()
{
	bIsClimbingLedge = false;
	bIsEscalating = false;
	PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	wallrunComponent->bIsDoingSomethingElse = false;
	applyingDecayingSpeed = true;
	DecayingSpeedTimeLeft = DecayingSpeedDuration;
	OnClimbStop.Broadcast();
}

void UWallClimbComponent::DetachFromWall()
{
	PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	playerController->ResetIgnoreLookInput();
	bIsClimbingLedge = false;
	bIsEscalating = false;
	CameraUp(false, 2);
	wallrunComponent->bIsDoingSomethingElse = true;
}




void UWallClimbComponent::Drop()
{
	
	DetachFromWall();
	bCancellingClimb = false;
	bIsPropulsed = true;
	PlayerCharacter->GetCharacterMovement()->AddImpulse(WallNormal * 5);
}

void UWallClimbComponent::CameraUp(bool CameraUp, float ResetTimeDilation)
{
	CameraStartPitch = playerController->GetControlRotation().Pitch;

	if (CameraUp) {
		playerController->SetIgnoreLookInput(true);
		CameraTargetPitch = CameraUpPitch;
		
	}
	else {
		
		CameraTargetPitch = 0;


	}
	CameraPitchRotationTimeLeft = 0.25f;
	bRotatingPitch = true;
}

void UWallClimbComponent::RotateCameraYaw()
{
	CameraStartYaw = playerController->GetControlRotation().Yaw;
	CameraYawTarget = (-WallNormal).ToOrientationRotator().Yaw;

	CameraYawRotationTimeLeft = CameraYawRotationDuration;
	bRotatingYaw = true;
}

void UWallClimbComponent::HeightTracer()
{
	if (bIsClimbingLedge) {
		return;
	}
	CalculateTracePoints();
	FVector RightVector = PlayerCharacter->GetActorRightVector() * SideTracersOffset;


	//right hand tracer
	FCollisionShape sphereShape;
	sphereShape.SetSphere(10);
	FHitResult hitResult;

	FVector start = StartTracePoint + RightVector;
	FVector end = EndTracePoint + RightVector;
	rightHandTracerTriggered=GetWorld()->SweepSingleByChannel(hitResult,start, end,FVector::UpVector.ToOrientationQuat(), ECC_GameTraceChannel2, sphereShape);
	canGrabRight = hitResult.ImpactNormal.Z > 0.8f;

	if (rightHandTracerTriggered){
		RightHandSpotToGrab = hitResult.ImpactPoint;
		RightHandNormal = hitResult.ImpactNormal;
		canGrabRight = RightHandNormal.Z > 0.8f;
		HitActor = hitResult.GetActor();
		locationHit = hitResult.Location;
	}
	else {
		canGrabRight = false;
	}

	
	//left hand tracer
	start = StartTracePoint - RightVector;
	end = EndTracePoint - RightVector;
	leftHandTracerTriggered = GetWorld()->SweepSingleByChannel(hitResult, start, end, FVector::UpVector.ToOrientationQuat(), ECC_GameTraceChannel2, sphereShape);

	if (leftHandTracerTriggered) {
		LeftHandSpotToGrab = hitResult.ImpactPoint;
		LeftHandNormal = hitResult.ImpactNormal;
		canGrabLeft = LeftHandNormal.Z > 0.8f;
		HitActor = hitResult.GetActor();
		locationHit = hitResult.Location;
	}else{
		canGrabLeft = false;
	}

	start = PlayerCharacter->GetActorLocation();
	end = start + PlayerCharacter->GetActorForwardVector()*75;

	bInFrontOfWall = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_GameTraceChannel2);
	bInFrontOfWall = bInFrontOfWall && hitResult.Actor == HitActor;

	FVector frontNormal = hitResult.ImpactNormal;

	float pelvisHeight = PlayerCharacter->GetActorLocation().Z;
	bool canGrab = (canGrabLeft && LeftHandSpotToGrab.Z - pelvisHeight < 30) || ( canGrabRight && RightHandSpotToGrab.Z - pelvisHeight < 30);

	if (bInFrontOfWall) {
		//calculate normals and hands positions
		WallNormal = frontNormal;
		FVector wallTangeant = WallNormal.RotateAngleAxis(-90, FVector(0, 0, 1)) * SideTracersOffset;
		WallTopLocation = canGrabLeft ? (LeftHandSpotToGrab + wallTangeant) : (RightHandSpotToGrab - wallTangeant);
		WallLocation = FVector(WallTopLocation.X, WallTopLocation.Y, PlayerCharacter->GetActorLocation().Z);
		WallLocation += WallNormal * 40;


		

		//UE_LOG(LogTemp, Warning, TEXT("left hand position : %s"), *LeftHandSpotToGrab.ToString());

		if (canGrab) {
			//climb ledge
			if ((canGrabLeft || canGrabRight) && !bIsClimbingLedge) {
				
				CalculateHandRotations();
				CorrectHandPositions(LeftHandRotY, RightHandRotY);
			
				
				
				ClimbMontageStartTime = UKismetMathLibrary::FClamp(HeightToClimb < 200 ? (1- HeightToClimb/200) : 0,0,0.75f);
				
				ClimbLedge();

			}
		}
		else {
			//escalate
			if (!bIsClimbingLedge && !bIsEscalating && PlayerCharacter->GetCharacterMovement()->IsFalling() && !bIsPropulsed && PlayerCharacter->GetVelocity().Z>-200) {
				LastWallJumped = HitActor;

				if ((WallNormal.Z >= -0.1f && WallNormal.Z<0.1f)) {
					EscalateWall();
				}

			}
		}
	}
	else {
		if (canGrab) {
			WallNormal = CalculateWallNormal();
			CalculateHandRotations();
			CorrectHandPositions(LeftHandRotY, RightHandRotY);

			ClimbMontageStartTime = UKismetMathLibrary::FClamp(HeightToClimb < 200 ? (1 - HeightToClimb / 200) : 0, 0, 0.75f);
			ClimbLedge();
		}
		else {
			if (bIsEscalating)
				StopClimbing();
		}
	}
}

bool UWallClimbComponent::ClampCamera(float Value)
{
	FTransform cameraTransform = PlayerCamera->GetComponentTransform();
	float relativeRotationYaw = UKismetMathLibrary::MakeRelativeTransform(cameraTransform, PlayerCharacter->GetActorTransform()).GetRotation().Rotator().Yaw;
	return (Value > 0 && Value + relativeRotationYaw < CameraAngleAllowed * 0.5f) || (Value < 0 && Value + relativeRotationYaw >= CameraAngleAllowed * -0.5f) || !bIsEscalating;
}

void UWallClimbComponent::CorrectHandPositions(float LeftHandYRotation, float RightHandYRotation)
{
	if (LeftHandYRotation >= 0) {
		LeftHandSpotToGrab -= FVector(0,0,UKismetMathLibrary::Abs(LeftHandYRotation) * HandPositionCorrectionMultiplier);
	}
	if (RightHandYRotation >= 0) {
		RightHandSpotToGrab -= FVector(0, 0, UKismetMathLibrary::Abs(RightHandYRotation) * HandPositionCorrectionMultiplier);
	}

	if (!rightHandTracerTriggered) {
		RightHandSpotToGrab = LeftHandSpotToGrab + PlayerCharacter->GetActorRightVector() * OneHandCorrectionOffset;
	}
	else {
		if (!leftHandTracerTriggered) {
			LeftHandSpotToGrab = RightHandSpotToGrab + PlayerCharacter->GetActorRightVector() * -OneHandCorrectionOffset;
		}
	}

	LeftHandSpotToGrab -= PlayerCharacter->GetActorForwardVector() * 20;
	RightHandSpotToGrab -= PlayerCharacter->GetActorForwardVector() * 20;
	LeftHandSpotToGrab += FVector(0, 0, -10);
	RightHandSpotToGrab += FVector(0, 0, -10);


}

void UWallClimbComponent::CalculateHandRotations()
{
	LeftHandRotY = UKismetMathLibrary::DegAcos(FVector::DotProduct(FVector::UpVector, LeftHandNormal)) * -UKismetMathLibrary::SignOfFloat(FVector::DotProduct(LeftHandNormal, PlayerCharacter->GetActorRightVector()));
	RightHandRotY = UKismetMathLibrary::DegAcos(FVector::DotProduct(FVector::UpVector, RightHandNormal)) * UKismetMathLibrary::SignOfFloat(FVector::DotProduct(RightHandNormal, PlayerCharacter->GetActorRightVector()));
}

FVector UWallClimbComponent::CalculateClimbPosition()
{
	FVector WallOffset = WallNormal * 22;
	return FVector(WallOffset.X + WallLocation.X, WallOffset.Y + WallLocation.Y, WallTopLocation.Z - 60);
}

void UWallClimbComponent::CalculateTracePoints()
{
	EndTracePoint = PlayerCharacter->GetActorLocation() + PlayerCharacter->GetActorForwardVector() * 75;
	StartTracePoint = EndTracePoint + FVector(0, 0, 200);
}

FVector UWallClimbComponent::CalculateWallNormal()
{
	FVector leftNormal = (PlayerCharacter->GetActorLocation() - LeftHandSpotToGrab).GetUnsafeNormal2D();

	FVector rightNormal = (PlayerCharacter->GetActorLocation() - RightHandSpotToGrab).GetUnsafeNormal2D();

	FVector average = (PlayerCharacter->GetActorLocation() - (RightHandSpotToGrab + LeftHandSpotToGrab) / 2).GetUnsafeNormal2D();

	return rightHandTracerTriggered ? (leftHandTracerTriggered ? average : rightNormal) : leftNormal;
	
}

