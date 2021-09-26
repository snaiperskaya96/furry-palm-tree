// Fill out your copyright notice in the Description page of Project Settings.


#include "RushGameState.h"

#include "Net/UnrealNetwork.h"

void ARushGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentObjective);
}
