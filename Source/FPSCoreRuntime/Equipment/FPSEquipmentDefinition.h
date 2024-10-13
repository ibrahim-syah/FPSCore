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
	bool IsFP;

	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<AMyWeaponActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttachTransform;

	UPROPERTY(EditAnywhere, Category = Equipment, meta=(EditCondition="IsFP"))
	FVector FP_OffsetRoot_LocationOffset;
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
	TArray<FMyEquipmentActorToSpawn> ActorsToSpawn_V2;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTransform GetFPAttachTransform() const { return ActorsToSpawn_V2.Num() > 1 ? ActorsToSpawn_V2[1].AttachTransform : FTransform(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetFPAttachSocket() const { return ActorsToSpawn_V2.Num() > 1 ? ActorsToSpawn_V2[1].AttachSocket : FName(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetFPOffsetRoot_LocationOffset() const { return ActorsToSpawn_V2.Num() > 1 ? ActorsToSpawn_V2[1].FP_OffsetRoot_LocationOffset : FVector(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSubclassOf<AMyWeaponActor> Get3PMeshClass() const { return ActorsToSpawn_V2.Num() > 1 ? ActorsToSpawn_V2[0].ActorToSpawn : NULL; }
};
