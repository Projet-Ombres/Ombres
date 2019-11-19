// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableNavLink.h"
#include "NavLinkCustomComponent.h"



AEditableNavLink::AEditableNavLink() {
	bSmartLinkIsRelevant = false;
	SetSmartLinkEnabled(false);

	SphereComponentsParent = CreateDefaultSubobject<USceneComponent>(FName("SceneComponentsParent"));
	
	
	ResetReferences();
	

	
}

void AEditableNavLink::SetSmartLinkPositions() {
	if (SphereCouples.Num() < PointLinks.Num()) {
		for (int i = 0, l = FMath::Abs(SphereCouples.Num() - PointLinks.Num()); i < l; i++) {
			FSphereCouple sphereCouple;
			FSphereCollision leftSphereCollision;
			USphereComponent* leftSphere = NewObject<USphereComponent>(this,  FName(*FString::Format(TEXT("Left[%d]"), TArray<FStringFormatArg>({i}))));
			leftSphereCollision.Index = i;
			leftSphereCollision.Left = true;
			leftSphereCollision.SphereComponent = leftSphere;

			leftSphere->AttachToComponent(SphereComponentsParent, FAttachmentTransformRules::KeepRelativeTransform);
			leftSphere->RegisterComponent();

			sphereCouple.leftSphere = leftSphereCollision;


			FScriptDelegate overlapDelegate;
			overlapDelegate.BindUFunction(this, FName("OnOverlap"));
			
			leftSphere->OnComponentBeginOverlap.Add(overlapDelegate);

			FSphereCollision rightSphereCollision;
			USphereComponent* rightSphere = NewObject<USphereComponent>(this, FName(*FString::Format(TEXT("Right[%d]"), TArray<FStringFormatArg>({ i }))));

			rightSphereCollision.Index = i;
			rightSphereCollision.Left = false;
			rightSphereCollision.SphereComponent = rightSphere;

			rightSphere->AttachToComponent(SphereComponentsParent, FAttachmentTransformRules::KeepRelativeTransform);
			rightSphere->RegisterComponent();

			rightSphere->OnComponentBeginOverlap.Add(overlapDelegate);

			sphereCouple.rightSphere = rightSphereCollision;
			SphereCouples.Add(sphereCouple);
		}
	}
	else if (SphereCouples.Num() > PointLinks.Num()) {
		for (int i = 0, l = FMath::Abs(SphereCouples.Num() - PointLinks.Num()); i < l; i++) {
			USphereComponent* leftSphereToDelete = SphereCouples[SphereCouples.Num() - 1].leftSphere.SphereComponent;
			USphereComponent* rightSphereToDelete = SphereCouples[SphereCouples.Num() - 1].rightSphere.SphereComponent;
			SphereCouples.RemoveAt(i);
			leftSphereToDelete->DestroyComponent();
			rightSphereToDelete->DestroyComponent();
		}
	}
	else {
		for (int i = 0, l = PointLinks.Num(); i < l; i++) {
			SphereCouples[i].leftSphere.SphereComponent->SetRelativeLocation(PointLinks[i].Left);
			SphereCouples[i].rightSphere.SphereComponent->SetRelativeLocation(PointLinks[i].Right);
			SphereCouples[i].leftSphere.SphereComponent->SetSphereRadius(PointLinks[i].SnapRadius);
			SphereCouples[i].rightSphere.SphereComponent->SetSphereRadius(PointLinks[i].SnapRadius);
		}
	}
}

void AEditableNavLink::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	FString sphereName = OverlappedComponent->GetName();
	int charPosition = sphereName.Find(TEXT("["));
	FString numberWithCrochet = sphereName.RightChop(charPosition);
	int index = FCString::Atoi(*numberWithCrochet.LeftChop(numberWithCrochet.Find(TEXT("]"))));

	if (sphereName.StartsWith(TEXT("Left"))) {
		OnPointReached(OtherActor,PointLinks[index].Right);
	}
	else {
		OnPointReached(OtherActor,PointLinks[index].Left);
	}


}



void AEditableNavLink::ResetReferences() {
	TArray<USceneComponent*> children;
	SphereComponentsParent->GetChildrenComponents(false,children);
	UE_LOG(LogTemp, Warning, TEXT("reset references : children count = %d"), children.Num());
	for (int i = 0, l = SphereCouples.Num(); i < l; i++) {
		SphereCouples[i].leftSphere.SphereComponent = Cast<USphereComponent>(children[i*2]);
		SphereCouples[i].leftSphere.SphereComponent = Cast<USphereComponent>(children[i * 2+1]);
	}
}
