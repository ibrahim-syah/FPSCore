// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FirstPersonComponent.h"
#include "Character/FPSPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TimelineComponent.h"
#include "Animation/LyraAnimInstance.h"


// Sets default values for this component's properties
UFirstPersonComponent::UFirstPersonComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	FP_Root = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Root"));
	FP_Root->SetupAttachment(this);

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

	FirstPersonLegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("FirstPersonLegMesh"));
	FirstPersonLegMesh->SetupAttachment(this);
	FirstPersonLegMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FirstPersonLegMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonLegMesh->bReceivesDecals = false;
	FirstPersonLegMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	FirstPersonLegMesh->CastShadow = false;
	FirstPersonLegMesh->SetVisibility(false, true);
	FirstPersonLegMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	FirstPersonLegMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));




	CrouchTL = CreateDefaultSubobject<UTimelineComponent>(FName("CrouchTL"));
	CrouchTL->SetTimelineLength(0.2f);
	CrouchTL->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	FOnTimelineFloat onCrouchTLCallback;
	onCrouchTLCallback.BindUFunction(this, FName{ TEXT("CrouchTLCallback") });
	CrouchAlphaCurve = CreateDefaultSubobject<UCurveFloat>(FName("CrouchAlphaCurve"));
	FKeyHandle KeyHandle = CrouchAlphaCurve->FloatCurve.AddKey(0.f, 0.f);
	CrouchAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = CrouchAlphaCurve->FloatCurve.AddKey(0.2f, 1.f);
	CrouchAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	CrouchTL->AddInterpFloat(CrouchAlphaCurve, onCrouchTLCallback);



	DipTL = CreateDefaultSubobject<UTimelineComponent>(FName("DipTL"));
	DipTL->SetTimelineLength(1.f);
	DipTL->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	FOnTimelineFloat onDipTLCallback;
	onDipTLCallback.BindUFunction(this, FName{ TEXT("DipTLCallback") });
	DipAlphaCurve = CreateDefaultSubobject<UCurveFloat>(FName("DipAlphaCurve"));
	KeyHandle = DipAlphaCurve->FloatCurve.AddKey(0.f, 0.f);
	DipAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = DipAlphaCurve->FloatCurve.AddKey(0.2f, 0.95f);
	DipAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = DipAlphaCurve->FloatCurve.AddKey(0.63f, 0.12f);
	DipAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = DipAlphaCurve->FloatCurve.AddKey(1.f, 0.f);
	DipAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	DipTL->AddInterpFloat(DipAlphaCurve, onDipTLCallback);

	WalkTL = CreateDefaultSubobject<UTimelineComponent>(FName("WalkTL"));
	WalkTL->SetTimelineLength(1.f);
	WalkTL->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	WalkTL->SetLooping(true);

	FOnTimelineFloat onWalkingLeftRightTLCallback;
	onWalkingLeftRightTLCallback.BindUFunction(this, FName{ TEXT("WalkLeftRightTLCallback") });
	WalkLeftRightAlphaCurve = CreateDefaultSubobject<UCurveFloat>(FName("WalkLeftRightAlphaCurve"));
	KeyHandle = WalkLeftRightAlphaCurve->FloatCurve.AddKey(0.f, 0.f);
	WalkLeftRightAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkLeftRightAlphaCurve->FloatCurve.AddKey(0.25f, 0.5f);
	WalkLeftRightAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkLeftRightAlphaCurve->FloatCurve.AddKey(0.5f, 1.f);
	WalkLeftRightAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkLeftRightAlphaCurve->FloatCurve.AddKey(0.75f, 0.5f);
	WalkLeftRightAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkLeftRightAlphaCurve->FloatCurve.AddKey(1.f, 0.f);
	WalkLeftRightAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	WalkTL->AddInterpFloat(WalkLeftRightAlphaCurve, onWalkingLeftRightTLCallback);

	FOnTimelineFloat onWalkingFwdBwdTLCallback;
	onWalkingFwdBwdTLCallback.BindUFunction(this, FName{ TEXT("WalkFwdBwdTLCallback") });
	WalkFwdBwdAlphaCurve = CreateDefaultSubobject<UCurveFloat>(FName("WalkFwdBwdAlphaCurve"));
	KeyHandle = WalkFwdBwdAlphaCurve->FloatCurve.AddKey(0.f, 0.f);
	WalkFwdBwdAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkFwdBwdAlphaCurve->FloatCurve.AddKey(0.3f, 1.f);
	WalkFwdBwdAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkFwdBwdAlphaCurve->FloatCurve.AddKey(0.5f, 0.f);
	WalkFwdBwdAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkFwdBwdAlphaCurve->FloatCurve.AddKey(0.8f, 1.f);
	WalkFwdBwdAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkFwdBwdAlphaCurve->FloatCurve.AddKey(1.f, 0.f);
	WalkFwdBwdAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	WalkTL->AddInterpFloat(WalkFwdBwdAlphaCurve, onWalkingFwdBwdTLCallback);

	FOnTimelineFloat onWalkingRollTLCallback;
	onWalkingRollTLCallback.BindUFunction(this, FName{ TEXT("WalkRollTLCallback") });
	WalkRollAlphaCurve = CreateDefaultSubobject<UCurveFloat>(FName("WalkRollAlphaCurve"));
	KeyHandle = WalkRollAlphaCurve->FloatCurve.AddKey(0.f, 0.18f);
	WalkRollAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkRollAlphaCurve->FloatCurve.AddKey(0.15f, 0.f);
	WalkRollAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkRollAlphaCurve->FloatCurve.AddKey(0.4f, 0.5f);
	WalkRollAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkRollAlphaCurve->FloatCurve.AddKey(0.65f, 1.f);
	WalkRollAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkRollAlphaCurve->FloatCurve.AddKey(0.9f, 0.5f);
	WalkRollAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = WalkRollAlphaCurve->FloatCurve.AddKey(1.f, 0.18f);
	WalkRollAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	WalkTL->AddInterpFloat(WalkRollAlphaCurve, onWalkingRollTLCallback);

	FOnTimelineEvent updateWalkEvent;
	updateWalkEvent.BindUFunction(this, FName{ TEXT("WalkTLUpdateEvent") });
	WalkTL->SetTimelinePostUpdateFunc(updateWalkEvent);

	ADSTL = CreateDefaultSubobject<UTimelineComponent>(FName("ADSTL"));
	ADSTL->SetTimelineLength(1.f);
	ADSTL->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	FOnTimelineFloat onADSTLCallback;
	onADSTLCallback.BindUFunction(this, FName{ TEXT("ADSTLCallback") });
	ADSAlphaCurve = CreateDefaultSubobject<UCurveFloat>(FName("ADSAlphaCurve"));
	KeyHandle = ADSAlphaCurve->FloatCurve.AddKey(0.f, 0.f);
	ADSAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = ADSAlphaCurve->FloatCurve.AddKey(0.25f, 0.05f);
	ADSAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = ADSAlphaCurve->FloatCurve.AddKey(0.75f, 0.95f);
	ADSAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	KeyHandle = ADSAlphaCurve->FloatCurve.AddKey(1.f, 1.f);
	ADSAlphaCurve->FloatCurve.SetKeyInterpMode(KeyHandle, ERichCurveInterpMode::RCIM_Cubic, /*auto*/true);
	ADSTL->AddInterpFloat(ADSAlphaCurve, onADSTLCallback);
}


// Called when the game starts
void UFirstPersonComponent::BeginPlay()
{
	Super::BeginPlay();

	if (MPC_FP)
	{
		MPC_FP_Instance = GetWorld()->GetParameterCollectionInstance(MPC_FP);
	}

	WalkTL->Play();
}


// Called every frame
void UFirstPersonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

////////////////////////////////////////////////////////////////////////////////// FP Procedural Animation Public Pure Getter (For AnimBP Property Access)

void UFirstPersonComponent::ResetVisibility(bool bShouldChangeTo1P)
{
	FirstPersonMesh->SetVisibility(bShouldChangeTo1P, true);
	FirstPersonLegMesh->SetVisibility(bShouldChangeTo1P, true);
}

void UFirstPersonComponent::ChangePOV(bool bShouldChangeTo1P)
{
	/*if (bShouldChangeTo1P)
	{
		FPCameraComponent->Activate();
	}
	else
	{
		FPCameraComponent->Deactivate();
	}*/
	FPCameraComponent->SetActive(bShouldChangeTo1P);
}

void UFirstPersonComponent::ResetAnimLayer_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ResetAnimLayer"));
}

void UFirstPersonComponent::InitializeAnimInstance(UAbilitySystemComponent* ASC)
{
	check(ASC);
	Cast<ULyraAnimInstance>(FirstPersonMesh->GetAnimInstance())->InitializeWithAbilitySystem(ASC);
	Cast<ULyraAnimInstance>(FirstPersonLegMesh->GetAnimInstance())->InitializeWithAbilitySystem(ASC);
}

FVector UFirstPersonComponent::GetLocationLagPos() const
{
	return LocationLagPos;
}

FVector UFirstPersonComponent::GetWalkAnimPos() const
{
	return WalkAnimPos;
}

FRotator UFirstPersonComponent::GetWalkAnimRot() const
{
	return WalkAnimRot;
}

float UFirstPersonComponent::GetWalkAnimAlpha() const
{
	return WalkAnimAlpha;
}

FVector UFirstPersonComponent::GetPitchOffsetPos() const
{
	return PitchOffsetPos;
}

FVector UFirstPersonComponent::GetCamRotOffset() const
{
	return CamRotOffset;
}

FRotator UFirstPersonComponent::GetCamRotCurrent() const
{
	return CamRotCurrent;
}

FRotator UFirstPersonComponent::GetCamRotRate() const
{
	return CamRotRate;
}

FRotator UFirstPersonComponent::GetInAirTilt() const
{
	return InAirTilt;
}

FVector UFirstPersonComponent::GetInAirOffset() const
{
	return InAirOffset;
}

FVector UFirstPersonComponent::GetCamOffsetCurrent() const
{
	return CamOffsetCurrent;
}

float UFirstPersonComponent::GetCamAnimAlpha() const
{
	return CamAnimAlpha;
}

float UFirstPersonComponent::GetADSAlpha() const
{
	return ADSAlpha;
}

float UFirstPersonComponent::GetADSAlphaInversed() const
{
	return ADSAlphaInversed;
}

float UFirstPersonComponent::GetADSAlphaLerp() const
{
	return ADSAlphaLerp;
}

float UFirstPersonComponent::GetHasWeaponAlpha() const
{
	return HasWeaponAlpha;
}

void UFirstPersonComponent::SetHasWeaponAlpha(float val)
{
	HasWeaponAlpha = FMath::Clamp(val, 0.f, 1.f);
}

float UFirstPersonComponent::GetCrouchAlpha() const
{
	return CrouchAlpha;
}

float UFirstPersonComponent::GetDipAlpha() const
{
	return DipAlpha;
}

void UFirstPersonComponent::SetDipStrength(float val)
{
	DipStrength = val;
}

FVector UFirstPersonComponent::GetADSOffset() const
{
	//return EquippedWeapon? EquippedWeapon->GetFirstPersonADSOffset() : FVector();;
	return CurrentWeaponADSOffset;
}

void UFirstPersonComponent::SetOffsetRootLocationOffset(FVector LocationOffset)
{
	Offset_Root->SetRelativeLocation(Offset_Root_LocationOffsetBase + LocationOffset);
}

void UFirstPersonComponent::Client_SetFPWeaponProps_Implementation(bool IsEquip, FVector NewADSOffset, FVector NewLocationOffset)
{
	IsEquip ? SetHasWeaponAlpha(1.f) : SetHasWeaponAlpha(0.f);
	CurrentWeaponADSOffset = NewADSOffset;
	SetOffsetRootLocationOffset(NewLocationOffset);
}

///////////////////////////////////////////////////////////// CROUCHING & SLIDING
void UFirstPersonComponent::CrouchTLCallback(float val)
{
	CrouchAlpha = val;
}

///////////////////////////////////////////////////////////// JUMPING & LANDING

void UFirstPersonComponent::Dip(float Speed, float Strength)
{
	// set dip param
	DipTL->SetPlayRate(Speed);
	DipStrength = Strength;
	DipTL->PlayFromStart();
}

void UFirstPersonComponent::DipTlCallback(float val)
{
	// update dip alpha
	DipAlpha = val * DipStrength;

	// update fp_root
	float lerpedZValue = FMath::Lerp(0.f, -10.f, DipAlpha);
	FVector newLocation = FVector(FP_Root->GetRelativeLocation().X, FP_Root->GetRelativeLocation().Y, lerpedZValue);
	FP_Root->SetRelativeLocation(newLocation);
}

void UFirstPersonComponent::LandingDip(float lastZVelocity, float jumpZVelocity)
{
	//float lastZVelocity = GetCharacterMovement()->GetLastUpdateVelocity().Z;
	float ZVectorLength = FVector(0.f, 0.f, lastZVelocity).Length();
	//float jumpZvelocity = GetCharacterMovement()->JumpZVelocity;
	float normalizedVelocity = UKismetMathLibrary::NormalizeToRange(ZVectorLength, 0.f, jumpZVelocity);
	float clampedVelocity = FMath::Clamp(normalizedVelocity, 0.f, 1.f);
	Dip(3.f, clampedVelocity);
}

///////////////////////////////////////////////////////////// WALKING

void UFirstPersonComponent::WalkLeftRightTLCallback(float val)
{
	WalkLeftRightAlpha = val;
}

void UFirstPersonComponent::WalkFwdBwdTLCallback(float val)
{
	WalkFwdBwdAlpha = val;
}

void UFirstPersonComponent::WalkRollTLCallback(float val)
{
	WalkRollAlpha = val;
}

void UFirstPersonComponent::WalkTLUpdateEvent()
{
	// update walk anim position
	float lerpedWalkAnimPosX = FMath::Lerp(-0.4f, 0.4f, WalkLeftRightAlpha);
	float lerpedWalkAnimPosZ = FMath::Lerp(-0.35f, 0.2f, WalkFwdBwdAlpha);
	WalkAnimPos = FVector(lerpedWalkAnimPosX, 0.f, lerpedWalkAnimPosZ);

	// update walk anim rotation
	float lerpedWalkAnimRotPitch = FMath::Lerp(1.f, -1.f, WalkRollAlpha);
	WalkAnimRot = FRotator(lerpedWalkAnimRotPitch, 0.f, 0.f);

	// get alpha of walking intensity
	float normalizedSpeed = UKismetMathLibrary::NormalizeToRange(OwningCharacter->GetVelocity().Length(), 0.f, OwningCharacter->GetBaseWalkSpeed());
	if (OwningCharacter->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		WalkAnimAlpha = 0.f;
	}
	else
	{
		WalkAnimAlpha = FMath::Clamp(normalizedSpeed, 0.f, 1.f); // had to clamp this because when sprinting, this would jump up beyond 1 and the footstep is too fast
	}

	float lerpedWalkAnimAlpha = FMath::Lerp(0.f, 1.65f, WalkAnimAlpha);
	WalkTL->SetPlayRate(lerpedWalkAnimAlpha);

	// update location lag vars
	UpdateVelocityVars();

	// update look input vars
	UpdateLookInputVars(CamRotCurrent);

	// camera animation
	FVector camOffset;
	float camAnimAlpha;
	ProcCamAnim(camOffset, camAnimAlpha);
}

void UFirstPersonComponent::UpdateVelocityVars()
{
	float velocityDotForwardVec = FVector::DotProduct(OwningCharacter->GetVelocity(), OwningCharacter->GetActorForwardVector());
	float velocityDotRightVec = FVector::DotProduct(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRightVector());
	float velocityDotUpVec = FVector::DotProduct(OwningCharacter->GetVelocity(), OwningCharacter->GetActorUpVector());

	float Y = velocityDotForwardVec / (OwningCharacter->GetBaseWalkSpeed() * -1.f);
	float X = velocityDotRightVec / OwningCharacter->GetBaseWalkSpeed();
	float Z = velocityDotUpVec / OwningCharacter->GetCharacterMovement()->JumpZVelocity * -1.f;

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

void UFirstPersonComponent::UpdateLookInputVars(FRotator CamRotPrev)
{
	// Step 1: determining how much to offset the viewmodel based
	// on our current camera pitch
	FRotator deltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(OwningCharacter->GetControlRotation(), OwningCharacter->GetActorRotation());
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

void UFirstPersonComponent::ProcCamAnim(FVector& CamOffsetArg, float& CamAnimAlphaArg)
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
void UFirstPersonComponent::ADSTLCallback(float val)
{
	ADSAlpha = val;
	ADSAlphaInversed = 1.f - ADSAlpha;
	ADSAlphaLerp = FMath::Lerp(0.2f, 1.f, ADSAlphaInversed);

	FPCameraComponent->SetFieldOfView(FMath::Lerp(FOVBase, FOVADS, ADSAlpha));
	FPCameraComponent->PostProcessSettings.VignetteIntensity = FMath::Lerp(0.4f, 0.7f, ADSAlpha);

	float lerpedFlatFov = FMath::Lerp(90.f, 25.f, ADSAlpha);
	MPC_FP_Instance->SetScalarParameterValue(FName("FOV"), lerpedFlatFov);
	FLinearColor OutColor;
	MPC_FP_Instance->GetVectorParameterValue(FName("Offset"), OutColor);
	float lerpedB = FMath::Lerp(0.f, 30.f, ADSAlpha);
	FLinearColor newColor = FLinearColor(OutColor.R, OutColor.G, lerpedB, OutColor.A);
	MPC_FP_Instance->SetVectorParameterValue(FName("Offset"), newColor);
}
////////////////////////////////////////////////////////////////////////////////////// END Procedural Anim

//void UFirstPersonComponent::AddDamageIndicator(FVector SourceLocation)
//{
//	Super::AddDamageIndicator(SourceLocation);
//
//	if (!GetWorldTimerManager().IsTimerActive(DamageFlinchTimer))
//	{
//		Dip(4.f, 0.5f);
//		GetWorldTimerManager().SetTimer(DamageFlinchTimer, this, &ThisClass::ClearDamageFlinchTimer, 0.3f, false);
//	}
//}

//void UFirstPersonComponent::ClearDamageFlinchTimer()
//{
//	GetWorldTimerManager().ClearTimer(DamageFlinchTimer);
//	DamageFlinchTimer.Invalidate();
//}
