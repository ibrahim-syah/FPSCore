// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacter.h"
#include "FPSPlayerCharacter.generated.h"

class UFirstPersonComponent;

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API AFPSPlayerCharacter : public ALyraCharacter
{
	GENERATED_BODY()

public:

	AFPSPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Cosmetics")
	void ResetVisibility(bool bShouldChangeTo1P);

	UFUNCTION(BlueprintCallable, Category = "FPSCore|FPSPlayerCharacter|Cosmetics")
	void ChangePOV(bool bShouldChangeTo1P);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	bool GetIsFirstPerson() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	float GetADSAlpha() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSCore|FPSPlayerCharacter|Procedural FP Animation")
	FORCEINLINE UFirstPersonComponent* GetFirstPersonComponent() { return FirstPersonComponent; }

	virtual void ToggleCrouch() override;
	UFUNCTION(BlueprintCallable)
	void ToggleCrouch_V2();

	FORCEINLINE float GetBaseWalkSpeed() const { return BaseWalkSpeed; }
	FORCEINLINE FVector GetStartingThirdPersonMeshLocation() const { return StartingThirdPersonMeshLocation; }
	FORCEINLINE float GetInvisibleBodyMeshOffsetLength() const { return InvisibleBodyMeshOffsetLength; }


protected:
	virtual void BeginPlay() override;

	virtual void OnAbilitySystemInitialized() override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UPROPERTY(EditAnywhere, Category = "FPSCore|FPSPlayerCharacter")
	TSubclassOf<UFirstPersonComponent> FPCompToSpawn;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFirstPersonComponent> FirstPersonComponent;
	///////////////////////////////////////////////////////////////////// Cosmetics
	//UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	//TArray<TObjectPtr<USkeletalMeshComponent>> TPCosmetics;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsFirstPerson = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPSCore|FPSPlayerCharacter")
	float BaseWalkSpeed{ 600.f };

	FVector StartingThirdPersonMeshLocation;
	float InvisibleBodyMeshOffsetLength{ -60.f };

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
};
