// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cosmetics/MyPawnComp_CharacterParts.h"
#include "Weapons/LyraRangedWeaponInstance.h"
#include "FPSRangedWeaponInstance.generated.h"

struct FMyAnimLayerSelectionSet;
struct FMyEquipmentActorToSpawn;

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UFPSRangedWeaponInstance : public ULyraRangedWeaponInstance
{
	GENERATED_BODY()

public:
	UFPSRangedWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SpawnEquipmentActors_V2(const TArray<FMyEquipmentActorToSpawn>& ActorsToSpawn);

	void Tick(float DeltaSeconds);


	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FMyAnimLayerSelectionSet EquippedAnimSet_V2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FMyAnimLayerSelectionSet UnequippedAnimSet_V2;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animation)
	void PickBestAnimLayer_V2(
		bool bEquipped,
		const FGameplayTagContainer& CosmeticTags,
		TSubclassOf<UAnimInstance>& TPFullBody_AnimLayer,
		TSubclassOf<UAnimInstance>& FPArms_AnimLayer,
		TSubclassOf<UAnimInstance>& FPLegs_AnimLayer
		) const;
};
