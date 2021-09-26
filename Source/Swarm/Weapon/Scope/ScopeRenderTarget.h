// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ScopeRenderTarget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SWARM_API UScopeRenderTarget : public USceneCaptureComponent2D
{
	GENERATED_BODY()
	
public:
	void AttachToSight(class USkeletalMeshComponent* Parent);
private:
	UPROPERTY()
	class USkeletalMeshComponent* ParentMesh = nullptr;
};
