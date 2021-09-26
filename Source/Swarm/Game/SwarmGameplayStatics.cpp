// Fill out your copyright notice in the Description page of Project Settings.


#include "SwarmGameplayStatics.h"

#include "SwarmGameInstance.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"

void USwarmGameplayStatics::PlayImpactFx(const UObject* WorldContextObject, const struct FHitResult& Hit)
{
	if (WorldContextObject && WorldContextObject->GetWorld())
	{
		if (USwarmGameInstance* Instance = WorldContextObject->GetWorld()->GetGameInstance<USwarmGameInstance>())
		{
			if (const FImpactFx* ImpactFx = Instance->ImpactFxs.Find(Hit.PhysMaterial.Get()))
			{
				if (ImpactFx->SoundEffect)
				{
					UGameplayStatics::PlaySoundAtLocation(WorldContextObject, ImpactFx->SoundEffect, Hit.Location);
				}

				if (ImpactFx->DecalMaterial && Hit.Component.IsValid())
				{
					//SpawnDecalAttached(class UMaterialInterface* DecalMaterial, FVector DecalSize, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, float LifeSpan = 0);
					if (UDecalComponent* Comp = UGameplayStatics::SpawnDecalAttached(ImpactFx->DecalMaterial, ImpactFx->DecalSize, Hit.Component.Get(), NAME_None, Hit.Location, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 60.f))
					{
						Comp->SetFadeScreenSize(ImpactFx->DecalFadeScreenSize);
					}
					//UGameplayStatics::SpawnDecalAtLocation(WorldContextObject, ImpactFx->DecalMaterial, ImpactFx->DecalSize, Hit.Location, Hit.ImpactNormal.Rotation(), );
				}
				
				if (ImpactFx->HitFx)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, ImpactFx->HitFx, Hit.Location, Hit.Normal.Rotation());
				}
			}
		}
	}
//	UGameplayStatics::PlaySoundAtLocation(WorldContextObject, )
}

FAccessorySlot USwarmGameplayStatics::GetAccessoryParentSlot(class UAccessory* Accessory)
{
	if (Accessory && Accessory->AssignedSlot && Accessory->AssignedSlot->ParentSlot)
	{
		return *Accessory->AssignedSlot->ParentSlot;
	}

	return FAccessorySlot();
}

FAccessorySlot USwarmGameplayStatics::GetAccessoryAssignedSlot(class UAccessory* Accessory)
{
	if (Accessory && Accessory->AssignedSlot)
	{
		return *Accessory->AssignedSlot;
	}

	return FAccessorySlot();
}
