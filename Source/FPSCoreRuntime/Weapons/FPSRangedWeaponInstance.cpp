// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSRangedWeaponInstance.h"
#include "Character/FPSPlayerCharacter.h"
#include "Weapons/MyWeaponActor.h"
#include "Equipment/FPSEquipmentDefinition.h"
#include "Net/UnrealNetwork.h"

UFPSRangedWeaponInstance::UFPSRangedWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//void UFPSRangedWeaponInstance::Tick(float DeltaSeconds)
//{
//	Super::Tick(DeltaSeconds);
//}

void UFPSRangedWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SpawnedActors_FP);
}

void UFPSRangedWeaponInstance::SpawnEquipmentActors_FP(const TArray<FMyEquipmentActorToSpawn>& ActorsToSpawn)
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
				AttachTarget = Char->GetFirstPersonMesh();
				Char->Client_SetFPWeaponProps(true, SpawnInfo.FP_ADSOffset, SpawnInfo.FP_OffsetRoot_LocationOffset); // Currently this means we can only have one fp actor for each equipment, because multiple actors mean the last actor will override the offsets
			}
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors_FP.Add(NewActor);
		}
	}
}

void UFPSRangedWeaponInstance::DestroyEquipmentActors()
{
	Super::DestroyEquipmentActors();

	for (AActor* Actor : SpawnedActors_FP)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void UFPSRangedWeaponInstance::PickBestAnimLayer_FP(
bool bEquipped,
		const FGameplayTagContainer& CosmeticTags,
		TSubclassOf<UAnimInstance>& FPArms_AnimLayer
) const
{
	const FLyraAnimLayerSelectionSet& SetToQuery_FP = (bEquipped ? EquippedAnimSet_FP : UnequippedAnimSet_FP);
	FPArms_AnimLayer = SetToQuery_FP.SelectBestLayer(CosmeticTags);
}
