// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Accessory.generated.h"

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	MainRail,
	MainRailEnd
};

UENUM(BlueprintType)
enum class EAccessoryType : uint8
{
	Sight,
	BackupRail
};

USTRUCT(BlueprintType)
struct FAccessorySlot
{
	GENERATED_BODY()
public:
	FName GetSocketName() const;
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EWeaponSlot Slot = EWeaponSlot::MainRail;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<class UAccessory> AccessoryClass = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UAccessory* Accessory = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USkeletalMeshComponent* Component = nullptr;

	struct FAccessorySlot* ParentSlot = nullptr;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SWARM_API UAccessory : public UObject
{
	GENERATED_BODY()
public:
	//UFUNCTION(BlueprintNativeEvent)
	virtual void OnEquipped(struct FAccessorySlot* AssignedSlot);

	UFUNCTION(BlueprintPure)
	FAccessorySlot GetParentSlot() const;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USkeletalMesh* Mesh = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EAccessoryType Type = EAccessoryType::Sight;

//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAccessorySlot* AssignedSlot = nullptr;
};
