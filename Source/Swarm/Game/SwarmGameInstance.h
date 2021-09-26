// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SwarmGameInstance.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FImpactFx
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class USoundCue *SoundEffect = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UMaterialInterface *DecalMaterial = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector DecalSize = FVector(10.f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float DecalFadeScreenSize = 0.01;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UNiagaraSystem *HitFx = nullptr;
};

/**
 * 
 */
UCLASS()
class SWARM_API USwarmGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<class UPhysicalMaterial *, FImpactFx> ImpactFxs;
};
