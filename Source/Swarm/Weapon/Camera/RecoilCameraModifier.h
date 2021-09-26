// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "RecoilCameraModifier.generated.h"

/**
 * 
 */
UCLASS()
class SWARM_API URecoilCameraModifier : public UCameraModifier
{
	GENERATED_BODY()
	
public:
	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	void PushRecoil(FRotator RecoilRotation);
	void RecoilRecoveryCheckpoint(class UWeapon* Weapon);
	void StartRecovery();
protected:
	bool bShouldRecover = false;
	FRotator ComulativeRecoil = FRotator::ZeroRotator;
	FRotator RecoilStartPoint = FRotator::ZeroRotator;

	UPROPERTY(Transient)
	class UWeapon* Weapon = nullptr;
};
