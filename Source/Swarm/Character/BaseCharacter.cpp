// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Swarm/Weapon/Weapon.h"
#include "Swarm/Weapon/Camera/RecoilCameraModifier.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "../AI/Controller/SwarmAIController.h"
#include "MeshUtilities.h"
#include "MeshMergeModule.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonSpringArm = CreateDefaultSubobject<USpringArmComponent>("FirstPersonSpringArm");
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ArmsMesh");
	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LegsMesh");
	PrimaryWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("PrimaryWeaponMesh");

	FirstPersonSpringArm->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetupAttachment(FirstPersonSpringArm);
	ArmsMesh->SetupAttachment(FirstPersonCamera);
	LegsMesh->SetupAttachment(GetCapsuleComponent());

	LegsMesh->bOnlyOwnerSee = true;
	LegsMesh->bOwnerNoSee = false;

	ArmsMesh->bOnlyOwnerSee = true;
	ArmsMesh->bOwnerNoSee = false;

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	FirstPersonSpringArm->bUsePawnControlRotation = true;
}

bool ABaseCharacter::IsSprinting() const
{
	return bIsSprinting && InputComponent->GetAxisValue("MoveForward") > 0.f;
}

void ABaseCharacter::EquipWeapon(class UWeapon* NewWeapon)
{
	if (Weapon)
	{
		Weapon->Destroy();
	}

	if (NewWeapon)
	{
		NewWeapon->OnEquipped(PrimaryWeaponMesh);
	}

	Weapon = NewWeapon;
}

bool ABaseCharacter::IsAlive() const
{
	return Health > 0.f;
}

float ABaseCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	Health -= Damage;

	if (Health <= 0.f)
	{
		if (Controller)
		{
			Controller->UnPossess();
		}

		TArray<USkeletalMeshComponent*> RagdollMeshes = { PrimaryWeaponMesh, GetMesh() };

		for (USkeletalMeshComponent* SkelMesh : RagdollMeshes)
		{
			SkelMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			SkelMesh->SetCollisionProfileName("Ragdoll");
			SkelMesh->SetAllBodiesSimulatePhysics(true);
			SkelMesh->SetSimulatePhysics(true);
			SkelMesh->WakeAllRigidBodies();
		}

		if (DamageEvent.IsOfType(1))
		{
			FPointDamageEvent const& PointDamageEvent = StaticCast<FPointDamageEvent const&>(DamageEvent);
			const FHitResult& Hit = PointDamageEvent.HitInfo;
			UE_LOG(LogTemp, Log, TEXT("%s"), *Hit.BoneName.ToString());
			GetMesh()->AddImpulse(PointDamageEvent.ShotDirection, Hit.BoneName);
		}

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ArmsMesh->DestroyComponent();
		LegsMesh->DestroyComponent();
	}

	return Damage;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//PrimaryWeaponMesh->SetupAttachment(ArmsMesh, "GripPoint");

	PrimaryWeaponMesh->AttachToComponent(IsLocallyControlled() && !Cast<ASwarmAIController>(Controller) ? ArmsMesh : GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "GripPoint");
	InitialArmsOffset = ArmsMesh->GetRelativeLocation();

	if (Weapon && !Weapon->AttachSocket.IsNone())
	{
		const FVector Offset = PrimaryWeaponMesh->GetSocketTransform(Weapon->AttachSocket, ERelativeTransformSpace::RTS_ParentBoneSpace).GetLocation();
		PrimaryWeaponMesh->SetRelativeLocation(-Offset);
	}
	else if (Weapon)
	{
		PrimaryWeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->PlayerCameraManager->AddNewCameraModifier(URecoilCameraModifier::StaticClass());
	}
}

void ABaseCharacter::OnLookUp(float Value)
{
	float Multiplier = 1.f;
	if (Weapon && bIsAiming)
	{
		Multiplier *= Weapon->GetControllerSpeedModifier();
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->AddPitchInput(Value * Multiplier);
	}
}

void ABaseCharacter::OnTurn(float Value)
{
	float Multiplier = 1.f;
	if (Weapon && bIsAiming)
	{
		Multiplier *= Weapon->GetControllerSpeedModifier();
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->AddYawInput(Value * Multiplier);
	}
}

void ABaseCharacter::OnMoveForward(float Value)
{
	if (UPawnMovementComponent* Movement = GetMovementComponent())
	{
		Movement->AddInputVector(GetActorForwardVector() * Value * (bIsAiming ? AimSpeedModifier : 1.f));
	}
}

void ABaseCharacter::OnMoveRight(float Value)
{
	if (UPawnMovementComponent* Movement = GetMovementComponent())
	{
		Movement->AddInputVector(GetActorRightVector() * Value * (bIsAiming ? AimSpeedModifier : 1.f));
	}
}

void ABaseCharacter::OnToggleAim()
{
	if (bIsAiming)
	{
		StopAim();
	}
	else
	{
		StartAim();
	}
}

void ABaseCharacter::StartAim()
{
	bIsAiming = true;

	if (Weapon)
	{
		Weapon->OnStartAiming();
	}
}

void ABaseCharacter::StopAim()
{
	bIsAiming = false;

	if (Weapon)
	{
		Weapon->OnStopAiming();
	}
}

void ABaseCharacter::OnTriggerDown()
{
	if (Weapon)
	{
		Weapon->OnTriggerDown();
	}
	bIsFiring = true;
}

void ABaseCharacter::OnTriggerUp()
{
	bIsFiring = false;

	if (Weapon)
	{
		Weapon->OnTriggerUp();
	}
}

void ABaseCharacter::OnSprintUp()
{
	bIsSprinting = false;
}

void ABaseCharacter::OnSprintDown()
{
	bIsSprinting = true;
}

void ABaseCharacter::OnToggleBackupSight()
{
	if (Weapon)
	{
		Weapon->CycleSight();
	}
}

void ABaseCharacter::OnFire()
{
	if (bIsFiring && Weapon)
	{
		Weapon->OnFire();
	}
}
#pragma optimize("", on)

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive())
	{
		return;
	}

	const FVector ArmsLocation = ArmsMesh->GetRelativeLocation();
	const FRotator ArmsRotation = ArmsMesh->GetRelativeRotation();

	if (bIsAiming && Weapon)
	{
		// Calculate the distance between the Sight socket and the camera location
		const FVector CameraLocation = FirstPersonSpringArm->GetRelativeLocation() + FirstPersonCamera->GetRelativeLocation();
		const FTransform SightSocketTransform = Weapon->GetSightActorTransform();
		const FVector Offset = SightSocketTransform.GetLocation() - ExpectedSightSocketLocation;
		// Find out where the arms mesh should be to align sight and camera
		const FVector TargetLocation = CameraLocation - SightSocketTransform.GetLocation() + ArmsLocation;
		// Grab a little bit of that and set it as new position
		const FVector NewLocation = UKismetMathLibrary::VLerp(ArmsLocation, TargetLocation, AdsTime * DeltaTime);
		ArmsMesh->SetRelativeLocation(NewLocation);

		// Find out if we need to rotate the arms as well and cache the initial arms location
		const FVector InitialLocation = Weapon->GetSightTransform().GetLocation();
		const float TargetPitch = Weapon->GetSightRotation().Pitch;
		const FRotator TargetRotation = FRotator(-TargetPitch, ArmsRotation.Yaw, ArmsRotation.Roll);
		// Rotate the arms a little bit 
		const FRotator NewRotation = UKismetMathLibrary::RLerp(ArmsRotation, TargetRotation, AdsTime * DeltaTime, true);
		ArmsMesh->SetRelativeRotation(NewRotation);
		// Calculate the difference between the new arms position and the old one	
		const FVector PostTransformLocation = Weapon->GetSightTransform().GetLocation();
		const FVector ArmsOffset = InitialLocation - PostTransformLocation;
		// Bring back the arms to the old position
		ArmsMesh->AddWorldOffset(ArmsOffset);

		ExpectedSightSocketLocation = SightSocketTransform.GetLocation() + (NewLocation - ArmsLocation);

		if (FirstPersonCamera)
		{
			FirstPersonCamera->FieldOfView = UKismetMathLibrary::Lerp(FirstPersonCamera->FieldOfView, Weapon->GetFov(), AdsTime * DeltaTime);
		}
	}
	else
	{
		ArmsMesh->SetRelativeLocation(UKismetMathLibrary::VLerp(ArmsLocation, InitialArmsOffset, AdsTime * DeltaTime));
		const FVector InitialLocation = Weapon->GetSightTransform().GetLocation();
		ArmsMesh->SetRelativeRotation(UKismetMathLibrary::RLerp(ArmsRotation, FRotator(0.f, ArmsRotation.Yaw, ArmsRotation.Roll), AdsTime * DeltaTime, true));
		const FVector PostTransformLocation = Weapon->GetSightTransform().GetLocation();
		const FVector ArmsOffset = InitialLocation - PostTransformLocation;
		//ArmsMesh->AddWorldOffset(-ArmsOffset);

		if (FirstPersonCamera)
		{
			FirstPersonCamera->FieldOfView = UKismetMathLibrary::Lerp(FirstPersonCamera->FieldOfView, BaseFov, AdsTime * DeltaTime);
		}
	}

	if (Weapon)
	{
		Weapon->Tick(DeltaTime);
	}

	// hack alert
	if (IsSprinting())
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, SprintSpeed, SprintAlpha);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, WalkSpeed, SprintAlpha);
	}
}
#pragma optimize("", off)

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::OnLookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::OnTurn);
	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::OnMoveRight);

	PlayerInputComponent->BindAction("ToggleAim", IE_Pressed, this, &ThisClass::OnToggleAim);
	PlayerInputComponent->BindAction("Trigger", IE_Pressed, this, &ThisClass::OnTriggerDown);
	PlayerInputComponent->BindAction("Trigger", IE_Released, this, &ThisClass::OnTriggerUp);
	PlayerInputComponent->BindAction("ToggleBackupSight", IE_Pressed, this, &ThisClass::OnToggleBackupSight);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ThisClass::OnSprintDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ThisClass::OnSprintUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

