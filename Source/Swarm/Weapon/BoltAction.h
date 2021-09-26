// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Swarm/Weapon/Weapon.h"
#include "BoltAction.generated.h"

/**
 * 
 */
UCLASS()
class SWARM_API UBoltAction : public UWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float Delta) override;
	virtual void OnFire() override;

	void LoadBolt();
	bool ShouldUseTimedBoltLoading() const { return !RifleBoltLoadingAnim || !ArmsBoltLoadingAnim; }

	virtual void OnTriggerUp() override;
	virtual void OnTriggerDown() override;
protected:
	bool bBoltLoaded = false;
	bool bBoltLoading = true;
	float BoltLoadingStartTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* ArmsBoltLoadingAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* RifleBoltLoadingAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BoltLoadingDuration = 2.f;
};
