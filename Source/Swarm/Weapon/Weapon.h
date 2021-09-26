// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Accessory.h"
#include "Swarm/Item/Item.h"

#include "UObject/NoExportTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SingleFire,
	Burst,
	Automatic
};

USTRUCT(BlueprintType)
struct FBulletData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FTransform SpawnTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadOnly)
	FTransform CurrentTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadOnly)
	float InitialSpeed = 400.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentSpeed = 400.f;
};

USTRUCT()
struct FSightData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	class USight *Sight = nullptr;

	UPROPERTY(Transient)
	FName SocketName = NAME_None;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SWARM_API UWeapon : public UItem
{
	GENERATED_BODY()

public:
	UWeapon();
	FVector FindBulletDirection() const;
	FTransform GetSightActorTransform() const;
	FTransform GetSightTransform() const;
	FRotator GetSightRotation() const;
	float GetFov() const;
	void CycleSight();

	virtual void OnTriggerDown();
	virtual void OnTriggerUp();
	virtual void Tick(float Delta);
	virtual void RegisterAccessory(FAccessorySlot &AccessorySlot);
	virtual void OnStartAiming();
	virtual void OnStopAiming();
	virtual void OnFire();
	virtual void Recover();

	float GetLastFireTime() const { return LastFireTime; }
	float GetRateOfFire() const { return RateOfFire; }
	float GetControllerSpeedModifier() const { return ControllerSpeedModifier; }

	UFUNCTION(BlueprintCallable)
	void EquipAccessoryFromClass(TSubclassOf<class UAccessory> AccessoryClass, EWeaponSlot Slot);
	UFUNCTION(BlueprintCallable)
	void RemoveAccessoryAtSlot(EWeaponSlot Slot);
	UFUNCTION(BlueprintCallable)
	virtual void OnEquipped(class USkeletalMeshComponent *Parent);
	UFUNCTION(BlueprintCallable)
	void Destroy();
	UFUNCTION(BlueprintCallable)
	UWeapon *Clone();
	UFUNCTION(BlueprintPure)
	class USkeletalMeshComponent *GetCurrentSightMesh() const;
	UFUNCTION(BlueprintPure)
	FAccessorySlot GetCurrentSightSlot() const;

public:
	float LastFireTime = 0.f;
	int32 RecoilBulletBurstCount = 0;
	bool bShouldRecover = false;
	bool bShouldFire = false;
	int32 SightIndex = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	EFireMode CurrentFireMode = EFireMode::SingleFire;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	float Damage = 10.f;

	UPROPERTY(BlueprintReadOnly, Category = "Recoil")
	FRandomStream RecoilStream;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	class UCurveFloat *VerticalRecoilIntensityPerBullet = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	class UCurveFloat *HorizontalRecoilIntensityPerBullet = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	int32 RecoilSeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float RecoilResetTime = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float MinVerticalRecoilRange = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float MaxVerticalRecoilRange = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float MinHorizontalRecoilRange = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float MaxHorizontalRecoilRange = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float RecoilRecoveryTime = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ads")
	float Fov = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ads")
	float ControllerSpeedModifier = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ads")
	TSubclassOf<class UMatineeCameraShake> FireCameraShake = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	class USoundCue *ShootAudioCue = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	class UParticleSystem *MuzzleFire = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RateOfFire = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName AttachSocket = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAccessorySlot> Accessories;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTraceTrajectory = false;

	UPROPERTY(Transient, BlueprintReadOnly)
	class USkeletalMeshComponent *ParentComponent = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly)
	class ABaseCharacter *OwningCharacter = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly)
	class USight *CurrentSight = nullptr;

	UPROPERTY()
	TArray<FBulletData> BulletPool;

	struct FAccessorySlot *CurrentSightSlot = nullptr;

	bool bUseBackupSight = false;
};
