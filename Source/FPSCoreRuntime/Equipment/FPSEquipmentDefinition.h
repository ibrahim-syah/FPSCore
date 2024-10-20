// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Templates/SubclassOf.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "FPSEquipmentDefinition.generated.h"

class AMyWeaponActor;

USTRUCT()
struct FMyEquipmentActorToSpawn
{
	GENERATED_BODY()

	FMyEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<AMyWeaponActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttachTransform;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FVector FP_OffsetRoot_LocationOffset;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FVector FP_ADSOffset;
};

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UFPSEquipmentDefinition : public ULyraEquipmentDefinition
{
	GENERATED_BODY()
	
public:
	UFPSEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<FMyEquipmentActorToSpawn> ActorsToSpawn_FP;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTransform GetFPAttachTransform() const { return ActorsToSpawn_FP.Num() > 0 ? ActorsToSpawn_FP[0].AttachTransform : FTransform(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetFPAttachSocket() const { return ActorsToSpawn_FP.Num() > 0 ? ActorsToSpawn_FP[0].AttachSocket : FName(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetFPOffsetRoot_LocationOffset() const { return ActorsToSpawn_FP.Num() > 0 ? ActorsToSpawn_FP[0].FP_OffsetRoot_LocationOffset : FVector(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSubclassOf<AActor> Get3PMeshClass() const { return ActorsToSpawn.Num() > 0 ? ActorsToSpawn[0].ActorToSpawn : NULL; }
};
