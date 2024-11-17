// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerCharacter.h"
#include "Character/LyraCharacterMovementComponent.h"
#include "Weapons/FPSRangedWeaponInstance.h"
#include "Camera/LyraCameraComponent.h"
#include "Player/LyraPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/FirstPersonComponent.h"



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

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	MeshComp->bCastHiddenShadow = true;
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	StartingThirdPersonMeshLocation = FVector(0.f, 0.f, -88.f);
}

void AFPSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FPCompToSpawn)
	{
		FirstPersonComponent = NewObject<UFirstPersonComponent>(this, FPCompToSpawn);
		FirstPersonComponent->RegisterComponent();
		FirstPersonComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		FirstPersonComponent->GetFirstPersonLegMesh()->SetRelativeLocation(StartingThirdPersonMeshLocation + FVector(InvisibleBodyMeshOffsetLength, 0.0f, 0.0f));
	}
}

void AFPSPlayerCharacter::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	/*if (FirstPersonComponent)
	{
		FirstPersonComponent->InitializeAnimInstance(ASC);
	}*/
}

void AFPSPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AFPSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	/*if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (PC->IsLocalController())
		{
		}
	}*/
}

void AFPSPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (FirstPersonComponent)
	{
		FirstPersonComponent->CrouchTL->Play();
	}
}

void AFPSPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (FirstPersonComponent)
	{
		FirstPersonComponent->CrouchTL->Reverse();
	}
}

void AFPSPlayerCharacter::ResetVisibility(bool bShouldChangeTo1P)
{
	GetMesh()->SetVisibility(!bShouldChangeTo1P, true);

	if (FirstPersonComponent)
	{
		FirstPersonComponent->ResetVisibility(bShouldChangeTo1P);
	}
}

void AFPSPlayerCharacter::ChangePOV(bool bShouldChangeTo1P)
{
	ALyraPlayerController* PC = GetLyraPlayerController();
	if (PC && PC->IsLocalPlayerController())
	{
		bIsFirstPerson = bShouldChangeTo1P;
		ResetVisibility(bShouldChangeTo1P);
		if (FirstPersonComponent)
		{
			FirstPersonComponent->ChangePOV(bShouldChangeTo1P);
		}
		CameraComponent->SetActive(!bShouldChangeTo1P);
		if (bShouldChangeTo1P)
		{
			// Move third person mesh back so that the shadow doesn't look disconnected
			GetMesh()->SetRelativeLocation(StartingThirdPersonMeshLocation + FVector(InvisibleBodyMeshOffsetLength, 0.0f, 0.0f));
			//CameraComponent->Deactivate();
		}
		else
		{
			// Reset the third person mesh
			GetMesh()->SetRelativeLocation(StartingThirdPersonMeshLocation);
			//CameraComponent->Activate();
		}
		PC->SetViewTarget(this);
	}
}

bool AFPSPlayerCharacter::GetIsFirstPerson() const
{
	return bIsFirstPerson;
}

float AFPSPlayerCharacter::GetADSAlpha() const
{
	return FirstPersonComponent ? FirstPersonComponent->GetADSAlpha() : 0.0f;
}

void AFPSPlayerCharacter::ToggleCrouch()
{
	return;
}

void AFPSPlayerCharacter::ToggleCrouch_V2()
{
	Super::ToggleCrouch();
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
	//LandingDip();

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
	//Dip(5.f, 1.f);

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