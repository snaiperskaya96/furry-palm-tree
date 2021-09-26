// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RushGameState.generated.h"

/**
 * 
 */
UCLASS()
class SWARM_API ARushGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	class ARushMarkerActor* CurrentObjective = nullptr;
};
