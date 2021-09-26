// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SwarmGameMode.h"
#include "SwarmHUD.h"
#include "SwarmCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASwarmGameMode::ASwarmGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	HUDClass = ASwarmHUD::StaticClass();
}
