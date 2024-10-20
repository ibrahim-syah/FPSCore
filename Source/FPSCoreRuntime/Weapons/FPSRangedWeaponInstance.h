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

	UFUNCTION(BlueprintPure, Category = Equipment)
	TArray<AActor*> GetSpawnedActors_FP() const { return SpawnedActors_FP; }

	virtual void SpawnEquipmentActors_FP(const TArray<FMyEquipmentActorToSpawn>& ActorsToSpawn);

	//void Tick(float DeltaSeconds);

	virtual void DestroyEquipmentActors() override;


	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FLyraAnimLayerSelectionSet EquippedAnimSet_FP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FLyraAnimLayerSelectionSet UnequippedAnimSet_FP;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animation)
	void PickBestAnimLayer_FP(
		bool bEquipped,
		const FGameplayTagContainer& CosmeticTags,
		TSubclassOf<UAnimInstance>& FPArms_AnimLayer
		) const;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors_FP;
};
