// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/LyraWeaponInstance.h"
#include "MyWeaponInstance.generated.h"

struct FMyAnimLayerSelectionSet;
struct FMyEquipmentActorToSpawn;
class AFPSPlayerCharacter;
class UMyPawnComp_CharacterParts;

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UMyWeaponInstance : public ULyraWeaponInstance
{
	GENERATED_BODY()
	
public:
	UMyWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = Equipment)
	TArray<AActor*> GetSpawnedActors_FP() const { return SpawnedActors_FP; }

	virtual void SpawnEquipmentActors_FP(const TArray<FMyEquipmentActorToSpawn>& ActorsToSpawn);

	virtual void DestroyEquipmentActors() override;

	virtual void OnLookInput(float deltaX, float deltaY);

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;



protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FLyraAnimLayerSelectionSet EquippedAnimSet_FP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FLyraAnimLayerSelectionSet UnequippedAnimSet_FP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UAnimMontage> WeaponEquipMontageTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UAnimMontage> WeaponUnequipMontageTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UAnimMontage> WeaponEquipMontageFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UAnimMontage> WeaponUnequipMontageFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UAnimMontage> WeaponQuickMeleeTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UAnimMontage> WeaponQuickMeleeFP;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer_FP(
		bool bEquipped,
		const FGameplayTagContainer& CosmeticTags
	) const;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors_FP;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	TObjectPtr<UMyPawnComp_CharacterParts> CosmeticComponent;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	FGameplayTagContainer CosmeticAnimStyleTags;

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	void DetermineCosmeticTags();

};
