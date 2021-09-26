// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Swarm/Weapon/Accessory.h"
#include "Sight.generated.h"

/**
 * 
 */
UCLASS()
class SWARM_API USight : public UAccessory
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	FRotator GetSightRotation() const;

	virtual void OnEquipped(struct FAccessorySlot* InAssignedSlot) override;

	virtual void OnActivated();
	virtual void OnDeactivated();
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bAlignWithWeapon = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Fov = 60.f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bRenderTarget = false;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<class UScopeRenderTarget> RenderTargetClass = nullptr;
	UPROPERTY(BlueprintReadOnly, Transient, VisibleAnywhere)
	class UScopeRenderTarget* RenderTarget = nullptr;
};
