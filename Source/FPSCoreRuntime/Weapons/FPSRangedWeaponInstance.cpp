// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSRangedWeaponInstance.h"
#include "Character/FPSPlayerCharacter.h"
#include "Weapons/MyWeaponActor.h"
#include "Equipment/FPSEquipmentDefinition.h"

UFPSRangedWeaponInstance::UFPSRangedWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFPSRangedWeaponInstance::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void UFPSRangedWeaponInstance::SpawnEquipmentActors_V2(const TArray<FMyEquipmentActorToSpawn>& ActorsToSpawn)
{
	
	if (APawn* OwningPawn = GetPawn())
	{

		for (const FMyEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			// Spawn and attach TP component to Fullbody mesh
			AMyWeaponActor* NewActor = GetWorld()->SpawnActorDeferred<AMyWeaponActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
			if (AFPSPlayerCharacter* Char = Cast<AFPSPlayerCharacter>(OwningPawn))
			{
				if (SpawnInfo.IsFP) {
					AttachTarget = Char->GetFirstPersonMesh();
					Char->Client_SetFPWeaponProps(true, SpawnInfo.FP_ADSOffset, SpawnInfo.FP_OffsetRoot_LocationOffset); // Currently this means we can only have one fp actor for each equipment, because multiple actors mean the last actor will override the offsets
				}
				else
				{
					AttachTarget = Char->GetMesh();
				}
			}
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void UFPSRangedWeaponInstance::PickBestAnimLayer_V2(
bool bEquipped,
		const FGameplayTagContainer& CosmeticTags,
		TSubclassOf<UAnimInstance>& TPFullBody_AnimLayer,
		TSubclassOf<UAnimInstance>& FPArms_AnimLayer,
		TSubclassOf<UAnimInstance>& FPLegs_AnimLayer
) const
{
	const FMyAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet_V2 : UnequippedAnimSet_V2);
	TPFullBody_AnimLayer = SetToQuery.SelectBestTPFullBodyLayer(CosmeticTags);
	FPArms_AnimLayer = SetToQuery.SelectBestFPArmsLayer(CosmeticTags);
	FPLegs_AnimLayer = SetToQuery.SelectBestFPLegsLayer(CosmeticTags);
}
