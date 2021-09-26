// Fill out your copyright notice in the Description page of Project Settings.


#include "Sight.h"

#include "Swarm/Weapon/Scope/ScopeRenderTarget.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"

FRotator USight::GetSightRotation() const
{
	if (AssignedSlot && AssignedSlot->ParentSlot && AssignedSlot->ParentSlot->Component)
	{
		return AssignedSlot->ParentSlot->Component->GetSocketTransform(AssignedSlot->Component->GetAttachSocketName(), RTS_Component).Rotator();
	}

	if (AssignedSlot)
	{
		return AssignedSlot->Component->GetRelativeRotation();
	}

	return FRotator::ZeroRotator;
}

void USight::OnEquipped(struct FAccessorySlot* InAssignedSlot)
{
	Super::OnEquipped(InAssignedSlot);

	if (bRenderTarget && RenderTargetClass)
	{
		RenderTarget = NewObject<UScopeRenderTarget>(InAssignedSlot->Component, RenderTargetClass);
		RenderTarget->AttachToSight(InAssignedSlot->Component);
		OnDeactivated();
	}

	InAssignedSlot->Component->bPerBoneMotionBlur = false;

	InAssignedSlot->Component->SetCastShadow(false);
}

void USight::OnActivated()
{
	if (bRenderTarget && RenderTarget)
	{
		RenderTarget->SetActive(true);
	}
}

void USight::OnDeactivated()
{
	if (bRenderTarget && RenderTarget)
	{
		if (RenderTarget->TextureTarget)
		{
			RenderTarget->TextureTarget->UpdateResourceImmediate();
		}
		RenderTarget->SetActive(false);
	}
}
