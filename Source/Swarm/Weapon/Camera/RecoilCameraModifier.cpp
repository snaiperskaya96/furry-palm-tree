// Fill out your copyright notice in the Description page of Project Settings.


#include "RecoilCameraModifier.h"

#include "Swarm/Weapon/Weapon.h"

#include "Camera/CameraTypes.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/KismetMathLibrary.h"

bool URecoilCameraModifier::ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV)
{
	bool Out = Super::ModifyCamera(DeltaTime, InOutPOV);

	if (InOutPOV.PreviousViewTransform.IsSet())
	{
		//UE_LOG(LogTemp, Log, TEXT("%s"), *InOutPOV.PreviousViewTransform.GetValue().Rotator().Add(-InOutPOV.Rotation.Pitch, -InOutPOV.Rotation.Yaw, -InOutPOV.Rotation.Roll).ToString());
	}

	return Out;
}

bool URecoilCameraModifier::ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);


	OutViewRotation.Add(ComulativeRecoil.Pitch, ComulativeRecoil.Yaw, 0.f);

	ComulativeRecoil.Pitch = FMath::Lerp(ComulativeRecoil.Pitch, 0.f, 0.5f);
	ComulativeRecoil.Yaw = FMath::Lerp(ComulativeRecoil.Yaw, 0.f, 0.5f);

	// Here we add the recoil independent offset that the player is pushing on the camera so the 
	// recovery doesn't go back to its origin point when the player moves the mouse up.
	RecoilStartPoint.Add(OutDeltaRot.Pitch >= 0.f ? OutDeltaRot.Pitch : 0.f, 0.f, 0.f);

	if (bShouldRecover)
	{
		if (RecoilStartPoint.Pitch >= OutViewRotation.Pitch)
		{
			bShouldRecover = false;
			RecoilStartPoint = FRotator::ZeroRotator;
		}
		else
		{
			RecoilStartPoint.Yaw = OutViewRotation.Yaw;
			RecoilStartPoint.Roll = OutViewRotation.Roll;

			const FRotator NewRotation = UKismetMathLibrary::RLerp(OutViewRotation, RecoilStartPoint, Weapon ? Weapon->RecoilRecoveryTime : 0.1f, true);
			OutViewRotation = NewRotation;
		}

		if (FMath::IsNearlyEqual(RecoilStartPoint.Pitch + 180.f, OutViewRotation.Pitch + 180.f, 0.3f) || RecoilStartPoint.Pitch >= OutViewRotation.Pitch)
		{
			bShouldRecover = false;
			//RecoilStartPoint = FRotator::ZeroRotator;
		}
	}

	return true;
}

void URecoilCameraModifier::PushRecoil(FRotator RecoilRotation)
{
	ComulativeRecoil.Add(RecoilRotation.Pitch, RecoilRotation.Yaw, RecoilRotation.Roll);
	bShouldRecover = false;
}

void URecoilCameraModifier::RecoilRecoveryCheckpoint(class UWeapon* FiringWeapon)
{
	RecoilStartPoint = CameraOwner->GetCameraRotation();
	Weapon = FiringWeapon;
}

void URecoilCameraModifier::StartRecovery()
{
	//if (CameraOwner->GetCameraRotation().Pitch < RecoilStartPoint.Pitch)
	{
		bShouldRecover = true;
	}
}
