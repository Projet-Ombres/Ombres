// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableNavLink.h"
#include "NavLinkCustomComponent.h"


AEditableNavLink::AEditableNavLink() {
	left = FVector(-50,0,0);
	right = FVector(50, 0, 0);
}

void AEditableNavLink::SetSmartLinkPositions() {
	
	SmartLinkComp->LinkRelativeStart = left;
	SmartLinkComp->LinkRelativeEnd = right;
	
}

void AEditableNavLink::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SetSmartLinkPositions();
}
