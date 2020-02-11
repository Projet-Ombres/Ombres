// Fill out your copyright notice in the Description page of Project Settings.


#include "AefenBlueprintLibrary.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

UHierarchicalInstancedStaticMeshComponent* UAefenBlueprintLibrary::AddHierarchicalInstancedStaticMeshComponent(AActor* target) {
	UHierarchicalInstancedStaticMeshComponent* component = NewObject<UHierarchicalInstancedStaticMeshComponent>(target, UHierarchicalInstancedStaticMeshComponent::StaticClass());
	if (component != nullptr) {
		component->RegisterComponent();
		component->SetupAttachment(target->GetRootComponent());
	}
	component->SetRelativeLocation(FVector::ZeroVector);
	return component;
}