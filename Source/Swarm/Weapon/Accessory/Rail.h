// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Swarm/Weapon/Accessory.h"
#include "Rail.generated.h"

/**
 * 
 */
UCLASS()
class SWARM_API URail : public UAccessory
{
	GENERATED_BODY()
	
public:
	virtual void OnEquipped(struct FAccessorySlot* InAssignedSlot) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAccessorySlot Slot;
};
