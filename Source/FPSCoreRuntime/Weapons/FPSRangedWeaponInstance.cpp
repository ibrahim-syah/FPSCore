// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSRangedWeaponInstance.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Physics/PhysicalMaterialWithTags.h"
#include "Character/FPSPlayerCharacter.h"
#include <random>

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPSRangedWeaponInstance)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_Weapon_SteadyAimingCamera, "Lyra.Weapon.SteadyAimingCamera");

UFPSRangedWeaponInstance::UFPSRangedWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HeatToHeatPerShotCurve.EditorCurveData.AddKey(0.0f, 1.0f);
	HeatToCoolDownPerSecondCurve.EditorCurveData.AddKey(0.0f, 2.0f);
}

void UFPSRangedWeaponInstance::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	UpdateDebugVisualization();
#endif
}

#if WITH_EDITOR
void UFPSRangedWeaponInstance::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateDebugVisualization();
}

void UFPSRangedWeaponInstance::UpdateDebugVisualization()
{
	ComputeHeatRange(/*out*/ Debug_MinHeat, /*out*/ Debug_MaxHeat);
	ComputeSpreadRange(/*out*/ Debug_MinSpreadAngle, /*out*/ Debug_MaxSpreadAngle);
	Debug_CurrentHeat = CurrentHeat;
	Debug_CurrentSpreadAngle = CurrentSpreadAngle;
	Debug_CurrentSpreadAngleMultiplier = CurrentSpreadAngleMultiplier;
}
#endif

void UFPSRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	// Start heat in the middle
	float MinHeatRange;
	float MaxHeatRange;
	ComputeHeatRange(/*out*/ MinHeatRange, /*out*/ MaxHeatRange);
	CurrentHeat = (MinHeatRange + MaxHeatRange) * 0.5f;

	// Derive spread
	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);

	// Default the multipliers to 1x
	CurrentSpreadAngleMultiplier = 1.0f;
	StandingStillMultiplier = 1.0f;
	JumpFallMultiplier = 1.0f;
	CrouchingMultiplier = 1.0f;
}

void UFPSRangedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

void UFPSRangedWeaponInstance::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* Pawn = GetPawn();
	check(Pawn != nullptr);

	const bool bMinSpread = UpdateSpread(DeltaSeconds);
	const bool bMinMultipliers = UpdateMultipliers(DeltaSeconds);

	bHasFirstShotAccuracy = bAllowFirstShotAccuracy && bMinMultipliers && bMinSpread;

	if (bIsRecoilActive)
	{
		Pawn->AddControllerPitchInput(RecoilPitchVelocity * DeltaSeconds);
		Pawn->AddControllerYawInput(RecoilYawVelocity * DeltaSeconds);

		RecoilPitchVelocity -= RecoilPitchDamping * DeltaSeconds;
		RecoilYawVelocity -= RecoilYawDamping * DeltaSeconds;

		if (RecoilPitchVelocity <= 0.0f)
		{
			bIsRecoilActive = false;
			StartRecoilRecovery();
		}
	}
	else if (bIsRecoilPitchRecoveryActive)
	{
		FRotator currentControlRotation = Pawn->GetControlRotation();

		FRotator deltaRot = currentControlRotation - RecoilCheckpoint;
		deltaRot.Normalize();

		if (deltaRot.Pitch > 1.f)
		{
			float interpSpeed = (1.f / DeltaSeconds) / 4.f;
			FRotator interpRot = FMath::RInterpConstantTo(currentControlRotation, RecoilCheckpoint, DeltaSeconds, interpSpeed);
			interpSpeed = (1.f / DeltaSeconds) / 10.f; // TODO: figure out how to make dynamic yaw recovery speed that depends on the pitch distance so that the pitch and yaw recovery ends together smoothly
			interpRot.Yaw = FMath::RInterpTo(currentControlRotation, RecoilCheckpoint, DeltaSeconds, interpSpeed).Yaw;
			if (!bIsRecoilYawRecoveryActive)
			{
				interpRot.Yaw = currentControlRotation.Yaw;
			}

			Pawn->GetController()->SetControlRotation(interpRot);
		}
		else if (deltaRot.Pitch > 0.1f)
		{
			float interpSpeed = (1.f / DeltaSeconds) / 6.f;
			FRotator interpRot = FMath::RInterpTo(currentControlRotation, RecoilCheckpoint, DeltaSeconds, interpSpeed);
			if (!bIsRecoilYawRecoveryActive)
			{
				interpRot.Yaw = currentControlRotation.Yaw;
			}
			Pawn->GetController()->SetControlRotation(interpRot);
		}
		else
		{
			bIsRecoilPitchRecoveryActive = false;
			bIsRecoilYawRecoveryActive = false;
			bIsRecoilNeutral = true;
		}
	}

#if WITH_EDITOR
	UpdateDebugVisualization();
#endif
}

void UFPSRangedWeaponInstance::ComputeHeatRange(float& MinHeat, float& MaxHeat)
{
	float Min1;
	float Max1;
	HeatToHeatPerShotCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min1, /*out*/ Max1);

	float Min2;
	float Max2;
	HeatToCoolDownPerSecondCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min2, /*out*/ Max2);

	float Min3;
	float Max3;
	HeatToSpreadCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min3, /*out*/ Max3);

	MinHeat = FMath::Min(FMath::Min(Min1, Min2), Min3);
	MaxHeat = FMath::Max(FMath::Max(Max1, Max2), Max3);
}

void UFPSRangedWeaponInstance::ComputeSpreadRange(float& MinSpread, float& MaxSpread)
{
	HeatToSpreadCurve.GetRichCurveConst()->GetValueRange(/*out*/ MinSpread, /*out*/ MaxSpread);
}

void UFPSRangedWeaponInstance::AddSpread()
{
	// Sample the heat up curve
	const float HeatPerShot = HeatToHeatPerShotCurve.GetRichCurveConst()->Eval(CurrentHeat);
	CurrentHeat = ClampHeat(CurrentHeat + HeatPerShot);

	// Map the heat to the spread angle
	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);

#if WITH_EDITOR
	UpdateDebugVisualization();
#endif
}

float UFPSRangedWeaponInstance::GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	const FRichCurve* Curve = DistanceDamageFalloff.GetRichCurveConst();
	return Curve->HasAnyData() ? Curve->Eval(Distance) : 1.0f;
}

float UFPSRangedWeaponInstance::GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	float CombinedMultiplier = 1.0f;
	if (const UPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UPhysicalMaterialWithTags>(PhysicalMaterial))
	{
		for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
		{
			if (const float* pTagMultiplier = MaterialDamageMultiplier.Find(MaterialTag))
			{
				CombinedMultiplier *= *pTagMultiplier;
			}
		}
	}

	return CombinedMultiplier;
}

bool UFPSRangedWeaponInstance::UpdateSpread(float DeltaSeconds)
{
	const float TimeSinceFired = GetWorld()->TimeSince(LastFireTime);

	if (TimeSinceFired > SpreadRecoveryCooldownDelay)
	{
		const float CooldownRate = HeatToCoolDownPerSecondCurve.GetRichCurveConst()->Eval(CurrentHeat);
		CurrentHeat = ClampHeat(CurrentHeat - (CooldownRate * DeltaSeconds));
		CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
	}

	float MinSpread;
	float MaxSpread;
	ComputeSpreadRange(/*out*/ MinSpread, /*out*/ MaxSpread);

	return FMath::IsNearlyEqual(CurrentSpreadAngle, MinSpread, KINDA_SMALL_NUMBER);
}

bool UFPSRangedWeaponInstance::UpdateMultipliers(float DeltaSeconds)
{
	const float MultiplierNearlyEqualThreshold = 0.05f;

	AFPSPlayerCharacter* EquippingCharacter = Cast<AFPSPlayerCharacter>(GetPawn());
	UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(EquippingCharacter->GetMovementComponent());

	// See if we are standing still, and if so, smoothly apply the bonus
	const float PawnSpeed = EquippingCharacter->GetVelocity().Size();
	const float MovementTargetValue = FMath::GetMappedRangeValueClamped(
		/*InputRange=*/ FVector2D(StandingStillSpeedThreshold, StandingStillSpeedThreshold + StandingStillToMovingSpeedRange),
		/*OutputRange=*/ FVector2D(SpreadAngleMultiplier_StandingStill, 1.0f),
		/*Alpha=*/ PawnSpeed);
	StandingStillMultiplier = FMath::FInterpTo(StandingStillMultiplier, MovementTargetValue, DeltaSeconds, TransitionRate_StandingStill);
	const bool bStandingStillMultiplierAtMin = FMath::IsNearlyEqual(StandingStillMultiplier, SpreadAngleMultiplier_StandingStill, SpreadAngleMultiplier_StandingStill * 0.1f);

	// See if we are crouching, and if so, smoothly apply the bonus
	const bool bIsCrouching = (CharMovementComp != nullptr) && CharMovementComp->IsCrouching();
	const float CrouchingTargetValue = bIsCrouching ? SpreadAngleMultiplier_Crouching : 1.0f;
	CrouchingMultiplier = FMath::FInterpTo(CrouchingMultiplier, CrouchingTargetValue, DeltaSeconds, TransitionRate_Crouching);
	const bool bCrouchingMultiplierAtTarget = FMath::IsNearlyEqual(CrouchingMultiplier, CrouchingTargetValue, MultiplierNearlyEqualThreshold);

	// See if we are in the air (jumping/falling), and if so, smoothly apply the penalty
	const bool bIsJumpingOrFalling = (CharMovementComp != nullptr) && CharMovementComp->IsFalling();
	const float JumpFallTargetValue = bIsJumpingOrFalling ? SpreadAngleMultiplier_JumpingOrFalling : 1.0f;
	JumpFallMultiplier = FMath::FInterpTo(JumpFallMultiplier, JumpFallTargetValue, DeltaSeconds, TransitionRate_JumpingOrFalling);
	const bool bJumpFallMultiplerIs1 = FMath::IsNearlyEqual(JumpFallMultiplier, 1.0f, MultiplierNearlyEqualThreshold);

	// Determine if we are aiming down sights, and apply the bonus based on how far into the ADS transition we are
	float AimingAlpha = EquippingCharacter->GetADSAlpha();
	const float AimingMultiplier = FMath::GetMappedRangeValueClamped(
		/*InputRange=*/ FVector2D(0.0f, 1.0f),
		/*OutputRange=*/ FVector2D(1.0f, SpreadAngleMultiplier_Aiming),
		/*Alpha=*/ AimingAlpha);
	const bool bAimingMultiplierAtTarget = FMath::IsNearlyEqual(AimingMultiplier, SpreadAngleMultiplier_Aiming, KINDA_SMALL_NUMBER);

	// Combine all the multipliers
	const float CombinedMultiplier = AimingMultiplier * StandingStillMultiplier * CrouchingMultiplier * JumpFallMultiplier;
	CurrentSpreadAngleMultiplier = CombinedMultiplier;

	// need to handle these spread multipliers indicating we are not at min spread
	return bStandingStillMultiplierAtMin && bCrouchingMultiplierAtTarget && bJumpFallMultiplerIs1 && bAimingMultiplierAtTarget;
}

float UFPSRangedWeaponInstance::SampleRecoilDirection(float x)
{
	// is it better to use a curve asset or just sample it directly from the function? idk man
	return FMath::Sin((x + 5.f) * (2.f * PI / 20.f)) * (100.f - x);
}

void UFPSRangedWeaponInstance::AddRecoil()
{
	if (bIsRecoilNeutral)
	{
		RecoilCheckpoint = GetPawn()->GetControlRotation();
		bIsRecoilNeutral = false;
	}
	if (bUpdateRecoilPitchCheckpointInNextShot)
	{
		RecoilCheckpoint = FRotator(GetPawn()->GetControlRotation().Pitch, RecoilCheckpoint.Yaw, RecoilCheckpoint.Roll);
		bUpdateRecoilPitchCheckpointInNextShot = false;
	}
	if (bUpdateRecoilYawCheckpointInNextShot)
	{
		RecoilCheckpoint = FRotator(RecoilCheckpoint.Pitch, GetPawn()->GetControlRotation().Yaw, RecoilCheckpoint.Roll);
		bUpdateRecoilYawCheckpointInNextShot = false;
	}
	StartRecoil();
}

void UFPSRangedWeaponInstance::OnLookInput(float deltaX, float deltaY)
{
	if (bIsRecoilPitchRecoveryActive)
	{
		FRotator currentRotation = GetPawn()->GetControlRotation();
		FRotator checkpointRotation = RecoilCheckpoint;

		FRotator deltaRot = (currentRotation - checkpointRotation).GetNormalized();

		if (deltaY > 0.f)
		{
			bIsRecoilPitchRecoveryActive = false;
			bIsRecoilNeutral = true;
			return;
		}

		if (deltaRot.Pitch < 0.f)
		{
			bUpdateRecoilPitchCheckpointInNextShot = true;
		}

		if (deltaX != 0.f)
		{
			if (bIsRecoilYawRecoveryActive)
			{
				bIsRecoilYawRecoveryActive = false;
			}

			bUpdateRecoilYawCheckpointInNextShot = true;
		}

	}
}

void UFPSRangedWeaponInstance::StartRecoil()
{
	InitialRecoilPitchForce = BaseRecoilPitchForce;
	InitialRecoilYawForce = BaseRecoilYawForce;

	AFPSPlayerCharacter* EquippingCharacter = Cast<AFPSPlayerCharacter>(GetPawn());
	if (bIsUseADSStabilizer)
	{
		CurrentADSHeat = EquippingCharacter->GetADSAlpha() > 0.f ? CurrentADSHeat + 1.f : 0.f;
		float ADSHeatModifier = FMath::Clamp(CurrentADSHeat / MaxADSHeat, 0.f, ADSHeatModifierMax);
		InitialRecoilPitchForce *= 1.f - ADSHeatModifier;
		InitialRecoilYawForce *= 1.f - ADSHeatModifier;
	}

	RecoilPitchVelocity = InitialRecoilPitchForce;
	RecoilPitchDamping = RecoilPitchVelocity / 0.1f;

	std::random_device rd;
	std::mt19937 gen(rd());
	float directionStat = SampleRecoilDirection(RecoilStat);
	float directionScaleModifier = directionStat / 100.f;
	float stddev = InitialRecoilYawForce * (1.f - RecoilStat / 100.f);

	std::normal_distribution<float> d(InitialRecoilYawForce * directionScaleModifier, stddev);
	RecoilYawVelocity = d(gen);
	RecoilYawDamping = (RecoilYawVelocity * -1.f) / 0.1f;

	bIsRecoilActive = true;
}

void UFPSRangedWeaponInstance::StartRecoilRecovery()
{
	bIsRecoilPitchRecoveryActive = true;
	bIsRecoilYawRecoveryActive = true;
}
