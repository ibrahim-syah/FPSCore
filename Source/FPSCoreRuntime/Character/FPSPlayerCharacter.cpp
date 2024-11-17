// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/LyraCharacterMovementComponent.h"
#include "Weapons/FPSRangedWeaponInstance.h"
#include "Camera/CameraComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Player/LyraPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/LyraAnimInstance.h"
#include "AGRPRO/Public/Components/AGR_CombatManager.h"
#include "Physics/LyraCollisionChannels.h"



AFPSPlayerCharacter::AFPSPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULyraCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(35.0f, 90.0f);

	ULyraCharacterMovementComponent* LyraMoveComp = CastChecked<ULyraCharacterMovementComponent>(GetCharacterMovement());
	LyraMoveComp->GravityScale = 1.5f;
	LyraMoveComp->MaxAcceleration = 3072.f;
	LyraMoveComp->BrakingFrictionFactor = 1.f;
	LyraMoveComp->PerchRadiusThreshold = 30.f;
	LyraMoveComp->bUseFlatBaseForFloorChecks = true;
	LyraMoveComp->JumpZVelocity = 750.f;
	LyraMoveComp->BrakingDecelerationFalling = 200.f;
	LyraMoveComp->AirControl = 0.275f;

	FP_Root = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Root"));
	FP_Root->SetupAttachment(RootComponent);

	Cam_Root = CreateDefaultSubobject<USpringArmComponent>(TEXT("Cam_Root"));
	Cam_Root->SetupAttachment(FP_Root);
	Cam_Root->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
	Cam_Root->TargetArmLength = 0;
	Cam_Root->bDoCollisionTest = false;
	Cam_Root->bUsePawnControlRotation = true;
	Cam_Root->bInheritPitch = true;
	Cam_Root->bInheritYaw = true;
	Cam_Root->bInheritRoll = false;

	Cam_Skel = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Cam_Skel"));
	Cam_Skel->SetupAttachment(Cam_Root);
	Cam_Skel->SetRelativeLocation(FVector(0.f, 0.f, -60.f));

	FPCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCameraComponent"));
	FPCameraComponent->SetRelativeLocation(FVector(0.f, 0.0f, 60.f));
	FPCameraComponent->SetupAttachment(Cam_Skel);
	FPCameraComponent->bUsePawnControlRotation = true;
	FPCameraComponent->PostProcessSettings.bOverride_VignetteIntensity = true;

	Mesh_Root = CreateDefaultSubobject<USpringArmComponent>(TEXT("Mesh_Root"));
	Mesh_Root->SetupAttachment(FP_Root);
	Mesh_Root->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
	Mesh_Root->TargetArmLength = 0;
	Mesh_Root->bDoCollisionTest = false;
	Mesh_Root->bUsePawnControlRotation = true;
	Mesh_Root->bInheritPitch = true;
	Mesh_Root->bInheritYaw = true;
	Mesh_Root->bInheritRoll = false;

	Offset_Root_LocationOffsetBase = FVector(0.f, 0.f, -60.f);
	Offset_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Offset_Root"));
	Offset_Root->SetupAttachment(Mesh_Root);
	Offset_Root->SetRelativeLocation(Offset_Root_LocationOffsetBase);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(Offset_Root);
	FirstPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->bReceivesDecals = false;
	FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetVisibility(false, true);
	FirstPersonMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	FirstPersonMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	MeshComp->bCastHiddenShadow = true;
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -88.f));

	FirstPersonLegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("FirstPersonLegMesh"));
	FirstPersonLegMesh->SetupAttachment(RootComponent);
	FirstPersonLegMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FirstPersonLegMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonLegMesh->bReceivesDecals = false;
	FirstPersonLegMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	FirstPersonLegMesh->CastShadow = false;
	FirstPersonLegMesh->SetVisibility(false, true);
	FirstPersonLegMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	FirstPersonLegMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));

	CombatManager = CreateDefaultSubobject<UAGR_CombatManager>(FName("CombatManager"));
	CombatManager->TraceChannel = UEngineTypes::ConvertToTraceType(Lyra_TraceChannel_Weapon);
	CombatManager->TraceSize = 30.f;
}

void AFPSPlayerCharacter::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	Cast<ULyraAnimInstance>(FirstPersonMesh->GetAnimInstance())->InitializeWithAbilitySystem(ASC);
	Cast<ULyraAnimInstance>(FirstPersonLegMesh->GetAnimInstance())->InitializeWithAbilitySystem(ASC);
}

void AFPSPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StartingThirdPersonMeshLocation = GetMesh()->GetRelativeLocation();
}

void AFPSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	/*APlayerState* PS = GetPlayerState();
	if (PS)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			WalkingTL->Play();
		}
	}*/
}

void AFPSPlayerCharacter::ChangePOV(bool bShouldChangeTo1P)
{
	ALyraPlayerController* PC = GetLyraPlayerController();
	if (PC && PC->IsLocalPlayerController())
	{
		FirstPersonMesh->SetVisibility(bShouldChangeTo1P, true);
		FirstPersonLegMesh->SetVisibility(bShouldChangeTo1P, true);
		GetMesh()->SetVisibility(!bShouldChangeTo1P, true);
		bIsFirstPerson = bShouldChangeTo1P;

		if (bShouldChangeTo1P)
		{
			// Move third person mesh back so that the shadow doesn't look disconnected
			GetMesh()->SetRelativeLocation(StartingThirdPersonMeshLocation + FVector(InvisibleBodyMeshOffsetLength, 0.0f, 0.0f));
			FirstPersonLegMesh->SetRelativeLocation(StartingThirdPersonMeshLocation + FVector(InvisibleBodyMeshOffsetLength, 0.0f, 0.0f));

			CameraComponent->Deactivate();
			FPCameraComponent->Activate();
		}
		else
		{
			// Reset the third person mesh
			GetMesh()->SetRelativeLocation(StartingThirdPersonMeshLocation);
			FirstPersonLegMesh->SetRelativeLocation(StartingThirdPersonMeshLocation);

			FPCameraComponent->Deactivate();
			CameraComponent->Activate();
		}
		PC->SetViewTarget(this);
	}
}

void AFPSPlayerCharacter::ToggleCrouch()
{
	return;
}

void AFPSPlayerCharacter::ToggleCrouch_V2()
{
	Super::ToggleCrouch();
}

////////////////////////////////////////////////////////////////////////////////// FP Procedural Animation Public Pure Getter (For AnimBP Property Access)


//void AFPSPlayerCharacter::SetEquippedWeapon(UFPSRangedWeaponInstance* Weapon)
//{
//	EquippedWeapon = Weapon;
//	HasWeaponAlpha = EquippedWeapon ? 1.f : 0.f;
//}

FVector AFPSPlayerCharacter::GetLocationLagPos() const
{
	return LocationLagPos;
}

FVector AFPSPlayerCharacter::GetWalkAnimPos() const
{
	return WalkAnimPos;
}

FRotator AFPSPlayerCharacter::GetWalkAnimRot() const
{
	return WalkAnimRot;
}

float AFPSPlayerCharacter::GetWalkAnimAlpha() const
{
	return WalkAnimAlpha;
}

FVector AFPSPlayerCharacter::GetPitchOffsetPos() const
{
	return PitchOffsetPos;
}

FVector AFPSPlayerCharacter::GetCamRotOffset() const
{
	return CamRotOffset;
}

FRotator AFPSPlayerCharacter::GetCamRotCurrent() const
{
	return CamRotCurrent;
}

FRotator AFPSPlayerCharacter::GetCamRotRate() const
{
	return CamRotRate;
}

FRotator AFPSPlayerCharacter::GetInAirTilt() const
{
	return InAirTilt;
}

FVector AFPSPlayerCharacter::GetInAirOffset() const
{
	return InAirOffset;
}

FVector AFPSPlayerCharacter::GetCamOffsetCurrent() const
{
	return CamOffsetCurrent;
}

float AFPSPlayerCharacter::GetCamAnimAlpha() const
{
	return CamAnimAlpha;
}

float AFPSPlayerCharacter::GetADSAlpha() const
{
	return ADSAlpha;
}

float AFPSPlayerCharacter::GetADSAlphaInversed() const
{
	return ADSAlphaInversed;
}

float AFPSPlayerCharacter::GetADSAlphaLerp() const
{
	return ADSAlphaLerp;
}

float AFPSPlayerCharacter::GetHasWeaponAlpha() const
{
	return HasWeaponAlpha;
}

void AFPSPlayerCharacter::SetHasWeaponAlpha(float val)
{
	HasWeaponAlpha = FMath::Clamp(val, 0.f, 1.f);
}

float AFPSPlayerCharacter::GetCrouchAlpha() const
{
	return CrouchAlpha;
}

float AFPSPlayerCharacter::GetDipAlpha() const
{
	return DipAlpha;
}

void AFPSPlayerCharacter::SetDipStrength(float val)
{
	DipStrength = val;
}

FVector AFPSPlayerCharacter::GetADSOffset() const
{
	//return EquippedWeapon? EquippedWeapon->GetFirstPersonADSOffset() : FVector();;
	return CurrentWeaponADSOffset;
}

bool AFPSPlayerCharacter::GetIsFirstPerson() const
{
	return bIsFirstPerson;
}

void AFPSPlayerCharacter::SetOffsetRootLocationOffset(FVector LocationOffset)
{
	Offset_Root->SetRelativeLocation(Offset_Root_LocationOffsetBase + LocationOffset);
}

void AFPSPlayerCharacter::Client_SetFPWeaponProps_Implementation(bool IsEquip, FVector NewADSOffset, FVector NewLocationOffset)
{
	IsEquip ? SetHasWeaponAlpha(1.f) : SetHasWeaponAlpha(0.f);
	CurrentWeaponADSOffset = NewADSOffset;
	SetOffsetRootLocationOffset(NewLocationOffset);
	//SetEquippedWeapon(Weapon);
}

///////////////////////////////////////////////////////////// CROUCHING & SLIDING
void AFPSPlayerCharacter::CrouchTLCallback(float val)
{
	CrouchAlpha = val;
}

///////////////////////////////////////////////////////////// JUMPING & LANDING

void AFPSPlayerCharacter::DipTlCallback(float val)
{
	// update dip alpha
	DipAlpha = val * DipStrength;

	// update fp_root
	float lerpedZValue = FMath::Lerp(0.f, -10.f, DipAlpha);
	FVector newLocation = FVector(FP_Root->GetRelativeLocation().X, FP_Root->GetRelativeLocation().Y, lerpedZValue);
	FP_Root->SetRelativeLocation(newLocation);
}

void AFPSPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		// change coyote time based on speed
		float normalizedSpeed = UKismetMathLibrary::NormalizeToRange(GetVelocity().Length(), 0.f, BaseWalkSpeed);
		float alpha = FMath::Clamp(normalizedSpeed, 0.f, 1.f);
		float lerpedValue = FMath::Lerp(0.25f, 1.f, alpha);
		float time = CoyoteTime * lerpedValue;
		GetWorldTimerManager().SetTimer(CoyoteTimerHandle, this, &ThisClass::CoyoteTimePassed, time, true);
	}
}

void AFPSPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	LandingDip();

	JumpsLeft = JumpsMax;

	// sequence 1
	// On landing, clear coyote timer
	GetWorld()->GetTimerManager().ClearTimer(CoyoteTimerHandle);
	CoyoteTimerHandle.Invalidate();
}

void AFPSPlayerCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	JumpsLeft = FMath::Clamp(JumpsLeft - 1, 0, JumpsMax);
	Dip(5.f, 1.f);

	// On jump, clear coyote timer
	GetWorld()->GetTimerManager().ClearTimer(CoyoteTimerHandle);
	CoyoteTimerHandle.Invalidate();
}

bool AFPSPlayerCharacter::CanJumpInternal_Implementation() const
{
	bool canJump = Super::CanJumpInternal_Implementation();
	float remainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(CoyoteTimerHandle);

	//bool isSlideTLActive = SlideTL->IsActive();
	//bool selected = isSlideTLActive ? SlideTL->GetPlaybackPosition() > 0.25f : true;
	//return (canJump || remainingTime > 0.f || JumpsLeft > 0) && (!isTimerActive && selected);
	return (canJump || remainingTime > 0.f || JumpsLeft > 0);
}

void AFPSPlayerCharacter::CoyoteTimePassed()
{
	JumpsLeft -= 1;
}

void AFPSPlayerCharacter::LandingDip()
{
	float lastZVelocity = GetCharacterMovement()->GetLastUpdateVelocity().Z;
	float ZVectorLength = FVector(0.f, 0.f, lastZVelocity).Length();
	float jumpZvelocity = GetCharacterMovement()->JumpZVelocity;
	float normalizedVelocity = UKismetMathLibrary::NormalizeToRange(ZVectorLength, 0.f, jumpZvelocity);
	float clampedVelocity = FMath::Clamp(normalizedVelocity, 0.f, 1.f);
	Dip(3.f, clampedVelocity);
	/*if (LandCue != nullptr)
	{
		float normalizedSpeed = UKismetMathLibrary::NormalizeToRange(GetVelocity().Length(), 0.f, BaseWalkSpeed);
		UGameplayStatics::PlaySoundAtLocation(this, LandCue, GetActorLocation(), clampedVelocity);
	}*/
}

///////////////////////////////////////////////////////////// WALKING

void AFPSPlayerCharacter::WalkLeftRightTLCallback(float val)
{
	WalkLeftRightAlpha = val;
}

void AFPSPlayerCharacter::WalkFwdBwdTLCallback(float val)
{
	WalkFwdBwdAlpha = val;
}

void AFPSPlayerCharacter::WalkRollTLCallback(float val)
{
	WalkRollAlpha = val;
}

void AFPSPlayerCharacter::WalkTLUpdateEvent()
{
	// update walk anim position
	float lerpedWalkAnimPosX = FMath::Lerp(-0.4f, 0.4f, WalkLeftRightAlpha);
	float lerpedWalkAnimPosZ = FMath::Lerp(-0.35f, 0.2f, WalkFwdBwdAlpha);
	WalkAnimPos = FVector(lerpedWalkAnimPosX, 0.f, lerpedWalkAnimPosZ);

	// update walk anim rotation
	float lerpedWalkAnimRotPitch = FMath::Lerp(1.f, -1.f, WalkRollAlpha);
	WalkAnimRot = FRotator(lerpedWalkAnimRotPitch, 0.f, 0.f);

	// get alpha of walking intensity
	float normalizedSpeed = UKismetMathLibrary::NormalizeToRange(GetVelocity().Length(), 0.f, BaseWalkSpeed);
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		WalkAnimAlpha = 0.f;
	}
	else
	{
		WalkAnimAlpha = FMath::Clamp(normalizedSpeed, 0.f, 1.f); // had to clamp this because when sprinting, this would jump up beyond 1 and the footstep is too fast
	}

	// update location lag vars
	UpdateVelocityVars();

	// update look input vars
	UpdateLookInputVars(CamRotCurrent);

	// camera animation
	FVector camOffset;
	float camAnimAlpha;
	ProcCamAnim(camOffset, camAnimAlpha);
}

void AFPSPlayerCharacter::UpdateVelocityVars()
{
	float velocityDotForwardVec = FVector::DotProduct(GetVelocity(), GetActorForwardVector());
	float velocityDotRightVec = FVector::DotProduct(GetVelocity(), GetActorRightVector());
	float velocityDotUpVec = FVector::DotProduct(GetVelocity(), GetActorUpVector());

	float Y = velocityDotForwardVec / (BaseWalkSpeed * -1.f);
	float X = velocityDotRightVec / BaseWalkSpeed;
	float Z = velocityDotUpVec / GetCharacterMovement()->JumpZVelocity * -1.f;

	FVector resultingVec = FVector(X, Y, Z);
	FVector scaledVec = resultingVec * 2.f;
	FVector ClampedVectorSize = scaledVec.GetClampedToSize(0.f, 4.f);

	float deltaTime = GetWorld()->DeltaTimeSeconds;
	float interpSpeed = (1.f / deltaTime) / 6.f;
	FVector interpedVec = FMath::VInterpTo(LocationLagPos, ClampedVectorSize, deltaTime, interpSpeed);
	LocationLagPos = interpedVec;

	FRotator oldRotator = Mesh_Root->GetRelativeRotation();
	float newRoll = FMath::Lerp(0.f, LocationLagPos.X * 5.f, ADSAlpha);
	Mesh_Root->SetRelativeRotation(FRotator(oldRotator.Pitch, oldRotator.Yaw, newRoll));

	interpSpeed = (1.f / deltaTime) / 12.f;
	FRotator targetRInterp = FRotator((LocationLagPos.Z * -2.f), 0.f, 0.f);
	FRotator interpedRot = FMath::RInterpTo(InAirTilt, targetRInterp, deltaTime, interpSpeed);
	InAirTilt = interpedRot;

	FVector targetVInterp = FVector((LocationLagPos.Z * -0.5f), 0.f, 0.f);
	FVector interpedInAirOffsetVec = FMath::VInterpTo(InAirOffset, targetVInterp, deltaTime, interpSpeed);
	InAirOffset = interpedInAirOffsetVec;
}

void AFPSPlayerCharacter::UpdateLookInputVars(FRotator CamRotPrev)
{
	// Step 1: determining how much to offset the viewmodel based
	// on our current camera pitch
	FRotator deltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	float normalizedPitch = UKismetMathLibrary::NormalizeToRange(deltaRotator.Pitch, -90.f, 90.f);
	float lerpedY = FMath::Lerp(3.f, -3.f, normalizedPitch);
	float lerpedZ = FMath::Lerp(2.f, -2.f, normalizedPitch);
	PitchOffsetPos = FVector(0.f, lerpedY, lerpedZ);

	float normalizedFurther = UKismetMathLibrary::NormalizeToRange(normalizedPitch, 0.f, 0.5f);
	float clampedNormalizedPitch = FMath::Clamp(normalizedFurther, 0.f, 1.f);
	float lerpedClampedNormalizedPitch = FMath::Lerp(15.f, 0.f, clampedNormalizedPitch);
	FVector newRelativeLocation = FVector(lerpedClampedNormalizedPitch, FP_Root->GetRelativeLocation().Y, FP_Root->GetRelativeLocation().Z);
	FP_Root->SetRelativeLocation(newRelativeLocation);


	// Step 2: finding the rotation rate of our camera and smoothing
	// the result to use for our weapon sway
	CamRotCurrent = FPCameraComponent->GetComponentRotation();
	FRotator deltaCamRot = UKismetMathLibrary::NormalizedDeltaRotator(CamRotCurrent, CamRotPrev);
	float deltaCamRotPitch, deltaCamRotYaw, deltaCamRotRoll;
	UKismetMathLibrary::BreakRotator(deltaCamRot, deltaCamRotRoll, deltaCamRotPitch, deltaCamRotYaw);
	float pitchInverse = deltaCamRotPitch * -1.f;
	float clampedPitchInverse = FMath::Clamp(pitchInverse, -5.f, 5.f);
	float clampedYaw = FMath::Clamp(deltaCamRotYaw, -5.f, 5.f);
	FRotator newRotator = FRotator(0.f, clampedYaw, clampedPitchInverse);
	float deltaSeconds = GetWorld()->DeltaTimeSeconds;
	//float weaponWeight = bHasWeapon ? FMath::Clamp(CurrentWeapon->WeaponSwaySpeed, 6.f, 80.f) : 6.f;
	//float interpSpeed = (1.f / deltaSeconds) / weaponWeight;
	float interpSpeed = (1.f / deltaSeconds) / 36.f;
	CamRotRate = UKismetMathLibrary::RInterpTo(CamRotRate, newRotator, deltaSeconds, interpSpeed);


	// Step 3: figuring out the amount to offset our viewmodel by,
	// in order to counteract the rotation of our weapon sway
	float normalizedRoll = UKismetMathLibrary::NormalizeToRange(CamRotRate.Roll, -5.f, 5.f);
	float lerpedRoll = FMath::Lerp(-10.f, 10.f, normalizedRoll);

	float normalizedYaw = UKismetMathLibrary::NormalizeToRange(CamRotRate.Yaw, -5.f, 5.f);
	float lerpedYaw = FMath::Lerp(-6.f, 6.f, normalizedYaw);
	CamRotOffset = FVector(lerpedYaw, 0.f, lerpedRoll);
}

void AFPSPlayerCharacter::ProcCamAnim(FVector& CamOffsetArg, float& CamAnimAlphaArg)
{
	FTransform spine_03_Transform = FirstPersonMesh->GetSocketTransform(FName("spine_03"));
	FTransform hand_r_Transform = FirstPersonMesh->GetSocketTransform(FName("hand_r"));
	FVector inversedTransformLocation = UKismetMathLibrary::InverseTransformLocation(spine_03_Transform, hand_r_Transform.GetLocation());
	FVector differenceVec = PrevHandLoc - inversedTransformLocation;
	FVector swappedAxesVec = FVector(differenceVec.Y, differenceVec.Z, differenceVec.X);
	CamOffset = swappedAxesVec * FVector(-1.f, 1.f, -1.f);
	FVector multipliedVec = CamOffset * CamStrength;
	PrevHandLoc = inversedTransformLocation;

	UAnimInstance* meshAnimInstance = FirstPersonMesh->GetAnimInstance();
	bool isAnyMontagePlaying = meshAnimInstance->IsAnyMontagePlaying();
	auto currentActiveMontage = meshAnimInstance->GetCurrentActiveMontage();
	bool isMontageActive = meshAnimInstance->Montage_IsActive(currentActiveMontage);
	float lowerSelectFloat = isMontageActive ? 1.f : 0.f;
	float upperSelectFloat = isAnyMontagePlaying ? lowerSelectFloat : 0.f;
	float deltaSeconds = GetWorld()->DeltaTimeSeconds;
	float interpSpeed = (1.f / deltaSeconds) / 24.f;
	FVector interpedVec = UKismetMathLibrary::VInterpTo(CamOffsetCurrent, multipliedVec, deltaSeconds, interpSpeed);
	CamOffsetCurrent = interpedVec.GetClampedToSize(0.f, 10.f);

	interpSpeed = (1.f / deltaSeconds) / 60.f;
	CamAnimAlpha = UKismetMathLibrary::FInterpTo(CamAnimAlpha, upperSelectFloat, deltaSeconds, interpSpeed);

	CamOffsetArg = CamOffsetCurrent;
	CamAnimAlphaArg = CamAnimAlpha;
}

///////////////////////////////////////////////////////////// CROUCHING & SLIDING
void AFPSPlayerCharacter::ADSTLCallback(float val)
{
	ADSAlpha = val;
	ADSAlphaInversed = 1.f - ADSAlpha;
	ADSAlphaLerp = FMath::Lerp(0.2f, 1.f, ADSAlphaInversed);

	FPCameraComponent->SetFieldOfView(FMath::Lerp(FOVBase, FOVADS, ADSAlpha));
	FPCameraComponent->PostProcessSettings.VignetteIntensity = FMath::Lerp(0.4f, 0.7f, ADSAlpha);
}
////////////////////////////////////////////////////////////////////////////////////// END Procedural Anim

//void AFPSPlayerCharacter::AddDamageIndicator(FVector SourceLocation)
//{
//	Super::AddDamageIndicator(SourceLocation);
//
//	if (!GetWorldTimerManager().IsTimerActive(DamageFlinchTimer))
//	{
//		Dip(4.f, 0.5f);
//		GetWorldTimerManager().SetTimer(DamageFlinchTimer, this, &ThisClass::ClearDamageFlinchTimer, 0.3f, false);
//	}
//}

void AFPSPlayerCharacter::ClearDamageFlinchTimer()
{
	GetWorldTimerManager().ClearTimer(DamageFlinchTimer);
	DamageFlinchTimer.Invalidate();
}
