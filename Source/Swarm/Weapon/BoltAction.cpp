// Fill out your copyright notice in the Description page of Project Settings.


#include "BoltAction.h"

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

void UBoltAction::OnFire()
{
	if (!bBoltLoaded)
	{
		if (!bBoltLoading)
		{
			LoadBolt();
		}
		return;
	}

	Super::OnFire();

	bBoltLoaded = false;
	bShouldFire = false;
}

void UBoltAction::LoadBolt()
{
	if (bBoltLoading || bBoltLoaded)
	{
		return;
	}

	BoltLoadingStartTime = GetWorld()->GetTimeSeconds();
	bBoltLoading = true;

	if (ShouldUseTimedBoltLoading())
	{
		//return;
	}

	if (ParentComponent && RifleBoltLoadingAnim)
	{
		ParentComponent->PlayAnimation(RifleBoltLoadingAnim, false);
	}

	if (USkeletalMeshComponent* Arms = Cast<USkeletalMeshComponent>(ParentComponent->GetAttachParent()))
	{
		if (ArmsBoltLoadingAnim)
		{
			Arms->PlayAnimation(ArmsBoltLoadingAnim, false);
		}
	}
}

void UBoltAction::OnTriggerUp()
{
	LoadBolt();
	Super::OnTriggerUp();
}

void UBoltAction::OnTriggerDown()
{
	if (bBoltLoading)
	{
		return;
	}

	Super::OnTriggerDown();
}

void UBoltAction::Tick(float Delta)
{
	Super::Tick(Delta);

	if (bBoltLoading)
	{
		const float TimeNeeded = RifleBoltLoadingAnim ? RifleBoltLoadingAnim->GetPlayLength() : BoltLoadingDuration;
		if (GetWorld()->GetTimeSeconds() - BoltLoadingStartTime >= TimeNeeded)
		{
			bBoltLoaded = true;
			bBoltLoading = false;
		}
	}
}
