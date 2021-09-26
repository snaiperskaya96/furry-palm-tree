// Fill out your copyright notice in the Description page of Project Settings.


#include "Accessory.h"

FName FAccessorySlot::GetSocketName() const
{
	return *UEnum::GetValueAsString(Slot).Replace(TEXT("EWeaponSlot::"), TEXT(""));
}

void UAccessory::OnEquipped(struct FAccessorySlot* InAssignedSlot)
{
	AssignedSlot = InAssignedSlot;
}

FAccessorySlot UAccessory::GetParentSlot() const
{
	if (AssignedSlot)
	{
		return *AssignedSlot;
	}

	return FAccessorySlot();
}
