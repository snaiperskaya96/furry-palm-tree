// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SWARM_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintPure)
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(class UWeapon* NewWeapon);

	UFUNCTION(BlueprintPure)
	bool IsAlive() const;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// axis
	void OnLookUp(float Value);
	void OnTurn(float Value);
	void OnMoveForward(float Value);
	void OnMoveRight(float Value);
	// actions
	void OnToggleAim();
	void StartAim();
	void StopAim();
	void OnTriggerDown();
	void OnTriggerUp();
	void OnSprintUp();
	void OnSprintDown();
	void OnToggleBackupSight();

	void OnFire();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	FVector ExpectedSightSocketLocation = FVector::ZeroVector;
	
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	class UWeapon* Weapon = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSprinting = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsAiming = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsFiring = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float AimSpeedModifier = 0.6f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float AdsTime = 1.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BaseFov = 90.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float SprintAlpha = 100.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float SprintSpeed = 500.f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float WalkSpeed = 350.f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Health = 100.f;
	UPROPERTY(BlueprintReadOnly)
	FVector InitialArmsOffset = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UCameraComponent* FirstPersonCamera = nullptr;	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USpringArmComponent* FirstPersonSpringArm = nullptr;	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USkeletalMeshComponent* ArmsMesh = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USkeletalMeshComponent* LegsMesh = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USkeletalMeshComponent* PrimaryWeaponMesh = nullptr;
};
