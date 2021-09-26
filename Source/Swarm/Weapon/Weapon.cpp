// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Accessory/Sight.h"

#include "Swarm/Game/SwarmGameplayStatics.h"
#include "Swarm/Character/BaseCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/RecoilCameraModifier.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Accessory/Rail.h"

UWeapon::UWeapon()
{
	BulletPool.Reserve(60);
}

void UWeapon::Tick(float Delta)
{
	if (bShouldFire)
	{
		if (GetWorld()->GetTimeSeconds() - GetLastFireTime() >= 60.f / GetRateOfFire())
		{
			OnFire();
		}
	}

	if (!bShouldRecover && GetWorld()->GetTimeSeconds() - LastFireTime >= RecoilResetTime)
	{
		Recover();
	}

}

void UWeapon::EquipAccessoryFromClass(TSubclassOf<class UAccessory> AccessoryClass, EWeaponSlot Slot)
{
	RemoveAccessoryAtSlot(Slot);
	for (FAccessorySlot& Accessory : Accessories)
	{
		if (Accessory.Slot == Slot)
		{
			Accessory.Accessory = NewObject<UAccessory>(ParentComponent, AccessoryClass);
			RegisterAccessory(Accessory);
			break;
		}
	}
}

void UWeapon::RemoveAccessoryAtSlot(EWeaponSlot Slot)
{
	for (FAccessorySlot& Accessory : Accessories)
	{
		if (Accessory.Accessory && Accessory.Slot == Slot)
		{
			if (Accessory.Component)
			{
				Accessory.Component->RemoveFromRoot();
				Accessory.Component->DestroyComponent();
			}

			Accessory.Accessory->MarkPendingKill();
			Accessory.Accessory = nullptr;
		}
	}
}

void UWeapon::OnEquipped(class USkeletalMeshComponent* Parent)
{
	ParentComponent = Parent;
	OwningCharacter = Cast<ABaseCharacter>(Parent->GetOwner());

	if (Parent)
	{
		Parent->SetSkeletalMesh(Mesh);
		if (!AttachSocket.IsNone())
		{
			const FVector Offset = Parent->GetSocketTransform(AttachSocket, ERelativeTransformSpace::RTS_ParentBoneSpace).GetLocation();
			Parent->SetRelativeLocation(-Offset);
		}
		else
		{
			Parent->SetRelativeLocation(FVector::ZeroVector);
		}

		for (FAccessorySlot& Slot : Accessories)
		{
			RegisterAccessory(Slot);
		}
	}

}

void UWeapon::Destroy()
{
	for (FAccessorySlot& Accessory : Accessories)
	{
		if (Accessory.Component)
		{
			Accessory.Component->SetSkeletalMesh(nullptr);
			//Accessory.Component->RemoveFromRoot();
			Accessory.Component->DestroyComponent(true);
		}
	}

	MarkPendingKill();
}

UWeapon* UWeapon::Clone()
{
	return DuplicateObject<UWeapon>(this, ParentComponent);
}

class USkeletalMeshComponent* UWeapon::GetCurrentSightMesh() const
{
	if (CurrentSightSlot)
	{
		return CurrentSightSlot->Component;
	}

	return nullptr;
}

FAccessorySlot UWeapon::GetCurrentSightSlot() const
{
	if (CurrentSightSlot)
	{
		return *CurrentSightSlot;
	}

	return FAccessorySlot();
}

void UWeapon::Recover()
{
	bShouldRecover = true;
	RecoilBulletBurstCount = 0;
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		if (URecoilCameraModifier* CameraRecoil = Cast<URecoilCameraModifier>(PC->PlayerCameraManager->FindCameraModifierByClass(URecoilCameraModifier::StaticClass())))
		{
			CameraRecoil->StartRecovery();
		}
	}
}

FTransform UWeapon::GetSightActorTransform() const
{
	if (CurrentSightSlot && CurrentSightSlot->Component)
	{
		if (bUseBackupSight && CurrentSightSlot->Component->DoesSocketExist("BackupSight"))
		{
			return CurrentSightSlot->Component->GetSocketTransform("BackupSight", RTS_Actor);
		}
		return CurrentSightSlot->Component->GetSocketTransform("Sight", RTS_Actor);
	}

	if (ParentComponent)
	{
		return ParentComponent->GetSocketTransform("Sight", RTS_Actor);
	}

	return FTransform::Identity;
}

FTransform UWeapon::GetSightTransform() const
{
	if (CurrentSightSlot && CurrentSightSlot->Component)
	{
		if (bUseBackupSight && CurrentSightSlot->Component->DoesSocketExist("BackupSight"))
		{
			return CurrentSightSlot->Component->GetSocketTransform("BackupSight", RTS_World);
		}
		return CurrentSightSlot->Component->GetSocketTransform("Sight", RTS_World);
	}

	if (ParentComponent)
	{
		return ParentComponent->GetSocketTransform("Sight", RTS_World);
	}

	return FTransform::Identity;
}

FRotator UWeapon::GetSightRotation() const
{
	if (CurrentSight)
	{
		return CurrentSight->GetSightRotation();
	}

	if (ParentComponent)
	{
		return ParentComponent->GetSocketTransform("Sight", RTS_Component).Rotator();
	}

	return FRotator::ZeroRotator;
}

float UWeapon::GetFov() const
{
	if (CurrentSight)
	{
		return CurrentSight->Fov;
	}

	return Fov;
}

void UWeapon::CycleSight()
{
	TArray<FAccessorySlot*> Slots;
	for (FAccessorySlot& Slot : Accessories)
	{
		if (Slot.Accessory && Slot.Accessory->IsA<USight>())
		{
			Slots.Add(&Slot);
		}

		if (URail* Rail = Cast<URail>(Slot.Accessory))
		{
			if (Rail->Slot.Accessory && Rail->Slot.Accessory->IsA<USight>())
			{
				Slots.Add(&Rail->Slot);
			}
		}
	}

	if (!bUseBackupSight && CurrentSightSlot)
	{
		bUseBackupSight = CurrentSightSlot->Component && CurrentSightSlot->Component->DoesSocketExist("BackupSight");
		if (bUseBackupSight)
		{
			return;
		}
	}

	bUseBackupSight = false;

	if (Slots.Num() == 0)
	{
		SightIndex = -1;
		CurrentSightSlot = nullptr;
		CurrentSight = nullptr;
	}
	else if (SightIndex >= Slots.Num())
	{
		SightIndex = 0;
	}
	else
	{
		SightIndex = SightIndex + 1 >= Slots.Num() ? 0 : SightIndex + 1;
	}


	CurrentSightSlot = Slots[SightIndex];
	CurrentSight = Cast<USight>(CurrentSightSlot->Accessory);
}

void UWeapon::OnTriggerDown()
{
	bShouldFire = true;
}

void UWeapon::OnTriggerUp()
{
	bShouldFire = false;
	Recover();
}

void UWeapon::RegisterAccessory(FAccessorySlot& Slot)
{
	if (Slot.Accessory == nullptr && Slot.AccessoryClass)
	{
		Slot.Accessory = NewObject<UAccessory>(this, Slot.AccessoryClass);
	}

	if (Slot.Accessory)
	{
		Slot.Component = NewObject<USkeletalMeshComponent>(ParentComponent);
		Slot.Component->AttachToComponent(ParentComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, Slot.GetSocketName());
		Slot.Component->SetRelativeLocation(FVector::ZeroVector);
		Slot.Component->RegisterComponent();
		Slot.Component->SetSkeletalMesh(Slot.Accessory->Mesh);
		Slot.Accessory->OnEquipped(&Slot);
	}

	if (!CurrentSight)
	{
		CycleSight();
	}
}

void UWeapon::OnStartAiming()
{
	if (CurrentSight)
	{
		CurrentSight->OnActivated();
	}
}

void UWeapon::OnStopAiming()
{
	if (CurrentSight)
	{
		CurrentSight->OnDeactivated();
	}
}

void UWeapon::OnFire()
{
	bShouldRecover = false;

	if (!ParentComponent)
	{
		return;
	}

	const FVector Direction = FindBulletDirection();

	const FVector StartPoint = ParentComponent->GetSocketLocation("Muzzle");
	const FVector Target = StartPoint + Direction * 10000 * 100;

	if (bTraceTrajectory)
	{
		DrawDebugDirectionalArrow(GetWorld(), StartPoint, Target, 2.f, FColor::Red, true, 100.f);
	}

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, Target, ECC_GameTraceChannel1))
	{
		if (Hit.Actor.IsValid() && Hit.Actor != ParentComponent->GetOwner())
		{
			FPointDamageEvent Event(Damage, Hit, Direction, nullptr);
			Hit.Actor->TakeDamage(Damage, Event, ParentComponent->GetOwner()->GetInstigatorController(), ParentComponent->GetOwner());
			USwarmGameplayStatics::PlayImpactFx(this, Hit);
		}
	}

	if (OwningCharacter && OwningCharacter->IsLocallyControlled())
	{
		APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController());
		
		if (GetWorld()->GetTimeSeconds() - LastFireTime >= RecoilResetTime)
		{
			RecoilStream = FRandomStream(RecoilSeed);
			RecoilBulletBurstCount = 0;
			if (URecoilCameraModifier* CameraRecoil = Cast<URecoilCameraModifier>(PC->PlayerCameraManager->FindCameraModifierByClass(URecoilCameraModifier::StaticClass())))
			{
				CameraRecoil->RecoilRecoveryCheckpoint(this);
			}
		}

		const float VerticalRecoilIntensityModifier = (VerticalRecoilIntensityPerBullet ? VerticalRecoilIntensityPerBullet->GetFloatValue(RecoilBulletBurstCount) : 1.f);
		const float HorizontalRecoilIntensityModifier = (HorizontalRecoilIntensityPerBullet ? HorizontalRecoilIntensityPerBullet->GetFloatValue(RecoilBulletBurstCount) : 1.f);
		const float VerticalRecoil = RecoilStream.FRandRange(MinVerticalRecoilRange, MaxVerticalRecoilRange) * VerticalRecoilIntensityModifier;
		const float HorizontalRecoil = RecoilStream.FRandRange(MinHorizontalRecoilRange, MaxHorizontalRecoilRange) * HorizontalRecoilIntensityModifier;

		if (URecoilCameraModifier* CameraRecoil = Cast<URecoilCameraModifier>(PC->PlayerCameraManager->FindCameraModifierByClass(URecoilCameraModifier::StaticClass())))
		{
			CameraRecoil->PushRecoil(FRotator(VerticalRecoil, HorizontalRecoil, 0.f));
		}

		if (FireCameraShake)
		{
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}

	RecoilBulletBurstCount++;

	if (ShootAudioCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootAudioCue, ParentComponent->GetSocketLocation("Muzzle"));
	}

	if (MuzzleFire)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFire, ParentComponent, "Muzzle", FVector::ZeroVector, FRotator::ZeroRotator);
	}

	if (CurrentFireMode == EFireMode::SingleFire)
	{
		bShouldFire = false;
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

FVector UWeapon::FindBulletDirection() const
{
	const FTransform SocketTransform = GetSightTransform();
	FVector Direction = SocketTransform.Rotator().Vector();

	// If the sight has a "sight align" socket we use the axis between Sight and SightAlign as direction for the bullet.
	// This is needed when the Sight has a weird angle over the optic
	if (CurrentSightSlot && CurrentSightSlot->Component)
	{
		if (CurrentSight->bAlignWithWeapon)
		{
			return (ParentComponent->GetSocketLocation("SightAlign") - CurrentSightSlot->Component->GetSocketLocation("Sight")).GetSafeNormal();
		}

		const FName AlignSocketName = (bUseBackupSight ? "BackupSightAlign" : "SightAlign");
		if (CurrentSightSlot->Component->DoesSocketExist(AlignSocketName))
		{
			const FVector AlignSocketLocation = CurrentSightSlot->Component->GetSocketLocation(AlignSocketName);
			const FVector SightSocketLocation = SocketTransform.GetLocation();
			return (AlignSocketLocation - SightSocketLocation).GetSafeNormal();
		}
	}

	return Direction;
}
