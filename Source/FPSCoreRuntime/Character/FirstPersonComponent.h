// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FirstPersonComponent.generated.h"

class UTimelineComponent;
class USpringArmComponent;
class UCameraComponent;
class AFPSPlayerCharacter;
class UAbilitySystemComponent;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSCORERUNTIME_API UFirstPersonComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFirstPersonComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter")
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter")
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonLegMesh() const { return FirstPersonLegMesh; }

	void ResetVisibility(bool bShouldChangeTo1P);
	void ChangePOV(bool bShouldChangeTo1P);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter")
	void ResetAnimLayer();

	void InitializeAnimInstance(UAbilitySystemComponent* ASC);

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

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	void SetOffsetRootLocationOffset(FVector NewLocationOffset);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_SetFPWeaponProps(bool IsEquip, FVector NewADSOffset, FVector NewLocationOffset);

protected:

	TObjectPtr<AFPSPlayerCharacter> OwningCharacter;

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> FP_Root;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USpringArmComponent> Mesh_Root;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Offset_Root;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVector Offset_Root_LocationOffsetBase;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonLegMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USpringArmComponent> Cam_Root = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Cam_Skel;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> FPCameraComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<TObjectPtr<USkeletalMeshComponent>> FPCosmetics;

	FVector CurrentWeaponADSOffset;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



//////////////////////////////////////////////////////////////////// Procedural Animation
public:
	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Dip")
	TObjectPtr<UTimelineComponent> CrouchTL;
	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Crouch")
	TObjectPtr<UCurveFloat> CrouchAlphaCurve;
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Crouch")
	void CrouchTLCallback(float val);



	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Dip")
	TObjectPtr<UTimelineComponent> DipTL;
	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Dip")
	TObjectPtr<UCurveFloat> DipAlphaCurve;
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Dip")
	void DipTlCallback(float val);




	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	TObjectPtr<UTimelineComponent> WalkTL;

	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	TObjectPtr<UCurveFloat> WalkLeftRightAlphaCurve;
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	void WalkLeftRightTLCallback(float val);

	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	TObjectPtr<UCurveFloat> WalkFwdBwdAlphaCurve;
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	void WalkFwdBwdTLCallback(float val);

	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	TObjectPtr<UCurveFloat> WalkRollAlphaCurve;
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	void WalkRollTLCallback(float val);
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|Walk")
	void WalkTLUpdateEvent();




	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|ADS")
	TObjectPtr<UTimelineComponent> ADSTL;
	UPROPERTY(BlueprintReadOnly, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|ADS")
	TObjectPtr<UCurveFloat> ADSAlphaCurve;
	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation|ADS")
	void ADSTLCallback(float val);

protected:

	////////// crouch & sliding
	float CrouchAlpha{ 0.f };

	////////// walking
	float HasWeaponAlpha;

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
	void UpdateLookInputVars(FRotator CamRotPrev);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ADS)
	TObjectPtr<UMaterialParameterCollection> MPC_FP;
	TObjectPtr<UMaterialParameterCollectionInstance> MPC_FP_Instance;

	// Landing and Dip
	void Dip(float Speed = 1.f, float Strength = 1.f);
	float DipStrength{ 1.f };
	float DipAlpha;
	void LandingDip(float lastZVelocity, float jumpZVelocity);


	/// flinch
	//virtual void AddDamageIndicator(FVector SourceLocation) override;
	//FTimerHandle DamageFlinchTimer;
	//virtual void ClearDamageFlinchTimer();
};
