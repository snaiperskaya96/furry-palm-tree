// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Swarm/Weapon/Accessory.h"
#include "SwarmGameplayStatics.generated.h"

/**
 * 
 */
UCLASS()
class SWARM_API USwarmGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    static void PlayImpactFx(const UObject* WorldContextObject, const struct FHitResult& Hit);

    UFUNCTION(BlueprintPure)
    static FAccessorySlot GetAccessoryParentSlot(class UAccessory* Accessory);

    UFUNCTION(BlueprintPure)
    static FAccessorySlot GetAccessoryAssignedSlot(class UAccessory* Accessory);
};
