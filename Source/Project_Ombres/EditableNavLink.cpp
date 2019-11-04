// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableNavLink.h"
#include "NavLinkCustomComponent.h"


AEditableNavLink::AEditableNavLink() {
	left = FVector(-50,0,0);
	right = FVector(50, 0, 0);
}

void AEditableNavLink::SetSmartLinkPositions() {
	GetSmartLinkComp()->SetLinkData(left, right, ENavLinkDirection::BothWays);
}

