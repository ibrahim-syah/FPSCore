// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacter.h"
#include "FPSPlayerCharacter.generated.h"

class UTimelineComponent;
class USpringArmComponent;
class UCameraComponent;
class UAGR_CombatManager;

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API AFPSPlayerCharacter : public ALyraCharacter
{
	GENERATED_BODY()

public:

	AFPSPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter")
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter")
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonLegMesh() const { return FirstPersonLegMesh; }

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Cosmetics")
	void ChangePOV(bool bShouldChangeTo1P);

	virtual void ToggleCrouch() override;
	UFUNCTION(BlueprintCallable)
	void ToggleCrouch_V2();

	UFUNCTION(BlueprintCallable)
	void RefreshMeshVisibility();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FVector GetLocationLagPos() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FVector GetWalkAnimPos() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FRotator GetWalkAnimRot() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetWalkAnimAlpha() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FVector GetPitchOffsetPos() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FVector GetCamRotOffset() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FRotator GetCamRotCurrent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FRotator GetCamRotRate() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FRotator GetInAirTilt() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FVector GetInAirOffset() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FVector GetCamOffsetCurrent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetCamAnimAlpha() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetADSAlpha() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetADSAlphaInversed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetADSAlphaLerp() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetHasWeaponAlpha() const;

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	void SetHasWeaponAlpha(float val);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetCrouchAlpha() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetDipAlpha() const;

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	void SetDipStrength(float val);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FVector GetADSOffset() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	bool GetIsFirstPerson() const;

	void SetFPWeaponProps(bool IsEquip, const FVector& NewADSOffset, const FVector& NewLocationOffset);

private:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	virtual void OnAbilitySystemInitialized() override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> FP_Root;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USpringArmComponent> Mesh_Root;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Offset_Root;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVector Offset_Root_LocationOffsetBase;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVector StartingThirdPersonMeshLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonLegMesh;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float InvisibleBodyMeshOffsetLength{ -60.f };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USpringArmComponent> Cam_Root = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Cam_Skel;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> FPCameraComponent;

	///////////////////////////////////////////////////////////////////// Cosmetics

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsFirstPerson = false;

	UPROPERTY(Replicated)
	FVector CurrentWeaponADSOffset;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponRelativeFPRootOffset)
	FVector CurrentWeaponRelativeFPRootOffset;

	UFUNCTION()
	void OnRep_CurrentWeaponRelativeFPRootOffset();

	///////////////////////////////////////////////////////////////////// AGR
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAGR_CombatManager> CombatManager;




	//////////////////////////////////////////////////////////////////// Procedural Animation
public:
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Crouch")
	void CrouchTLCallback(float val);

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Dip")
	void DipTlCallback(float val);

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	void WalkLeftRightTLCallback(float val);

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	void WalkFwdBwdTLCallback(float val);
	
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	void WalkRollTLCallback(float val);
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	void WalkTLUpdateEvent();

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|ADS")
	void ADSTLCallback(float val);

protected:

	////////// crouch & sliding
	float CrouchAlpha{ 0.f };

	////////// walking
	UPROPERTY(Replicated)
	float HasWeaponAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed{ 600.f };

	float WalkFwdBwdAlpha;
	float WalkLeftRightAlpha;
	float WalkRollAlpha;

	FVector WalkAnimPos;
	FRotator WalkAnimRot;
	float WalkAnimAlpha;
	FVector LocationLagPos;
	void UpdateVelocityVars();

	FVector PitchOffsetPos;
	FVector CamRotOffset;
	FRotator CamRotCurrent;
	FRotator CamRotRate;
	FVector InAirOffset;
	FRotator InAirTilt;
	void UpdateLookInputVars(const FRotator& CamRotPrev);

	void ProcCamAnim(FVector& CamOffsetArg, float& CamAnimAlphaArg);
	FVector PrevHandLoc;
	FVector CamOffset;
	float CamStrength{ 25.f };
	FVector CamOffsetCurrent;
	float CamAnimAlpha{ 0.f };

	/// ADS
	float ADSAlpha{ 0.f };
	float ADSAlphaInversed{ 1.f }; // basically 1 - ADSAlpha
	float ADSAlphaLerp{ 1.f }; // Same thing as ADSAlphaInversed, but clamped between 0.2f - 1.f
	float FOVBase{ 90.f };
	float FOVADS{ 70.f };

	/// Jump
	int32 JumpsLeft{ 2 };
	int32 JumpsMax{ 2 };

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnJumped_Implementation() override;
	virtual bool CanJumpInternal_Implementation() const override;

	FTimerHandle CoyoteTimerHandle;
	void CoyoteTimePassed();
	float CoyoteTime{ 0.35f };

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Jump")
	void Dip(float Speed = 1.f, float Strength = 1.f);
	float DipStrength{ 1.f };
	float DipAlpha;
	void LandingDip();


	/// flinch
	//virtual void AddDamageIndicator(FVector SourceLocation) override;
	FTimerHandle DamageFlinchTimer;
	virtual void ClearDamageFlinchTimer();
	
};
