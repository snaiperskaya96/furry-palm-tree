// Fill out your copyright notice in the Description page of Project Settings.


#include "ScopeRenderTarget.h"

#include "Components/SkeletalMeshComponent.h"

void UScopeRenderTarget::AttachToSight(class USkeletalMeshComponent* Parent)
{
	ParentMesh = Parent;
	AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, "RenderTarget");
	SetRelativeLocation(FVector::ZeroVector);
	RegisterComponent();
}
