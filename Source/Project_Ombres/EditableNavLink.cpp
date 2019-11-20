// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableNavLink.h"
#include "NavLinkCustomComponent.h"



AEditableNavLink::AEditableNavLink() {
	left = FVector(0, -50, 0);
	right = FVector(0, 50, 0);
	PointLinks.Empty();
	SetSmartLinkEnabled(true);
}

void AEditableNavLink::SetSmartLinkPositions() {
	GetSmartLinkComp()->SetLinkData(left, right, ENavLinkDirection::BothWays);
}