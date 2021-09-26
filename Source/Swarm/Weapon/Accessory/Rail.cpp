// Fill out your copyright notice in the Description page of Project Settings.


#include "Rail.h"
#include "Components/SkeletalMeshComponent.h"

void URail::OnEquipped(struct FAccessorySlot* InAssignedSlot)
{
	Super::OnEquipped(InAssignedSlot);
	if (Slot.Accessory == nullptr && Slot.AccessoryClass)
	{
		Slot.Accessory = NewObject<UAccessory>(GetOuter(), Slot.AccessoryClass);
	}

	if (Slot.Accessory)
	{
		Slot.Component = NewObject<USkeletalMeshComponent>(InAssignedSlot->Component);
		Slot.Component->AttachToComponent(InAssignedSlot->Component, FAttachmentTransformRules::SnapToTargetIncludingScale, Slot.GetSocketName());
		Slot.Component->SetRelativeLocation(FVector::ZeroVector);
		Slot.Component->RegisterComponent();
		Slot.Component->SetSkeletalMesh(Slot.Accessory->Mesh);
		Slot.ParentSlot = InAssignedSlot;
		Slot.Accessory->OnEquipped(&Slot);
	}
}
