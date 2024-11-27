// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/MyWeaponInstance.h"
#include "Cosmetics/MyPawnComp_CharacterParts.h"
#include "Character/FPSPlayerCharacter.h"
#include "Weapons/MyWeaponActor.h"
#include "Equipment/FPSEquipmentDefinition.h"
#include "Net/UnrealNetwork.h"

UMyWeaponInstance::UMyWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyWeaponInstance::Tick(float DeltaSeconds)
{
}

void UMyWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SpawnedActors_FP);
}

void UMyWeaponInstance::SpawnEquipmentActors_FP(const TArray<FMyEquipmentActorToSpawn>& ActorsToSpawn)
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
				Char->SetFPWeaponProps(true, SpawnInfo.FP_ADSOffset, SpawnInfo.FP_OffsetRoot_LocationOffset); // Currently this means we can only have one fp actor for each equipment, because multiple actors mean the last actor will override the offsets
			}
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors_FP.Add(NewActor);
		}
	}
}

void UMyWeaponInstance::DestroyEquipmentActors()
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

void UMyWeaponInstance::OnLookInput(float deltaX, float deltaY)
{
}

void UMyWeaponInstance::OnEquipped()
{
	AFPSPlayerCharacter* Char = Cast<AFPSPlayerCharacter>(GetPawn());
	AController* PC = Char->GetController();
	if (PC && PC->IsLocalPlayerController())
	{
		Char->RefreshMeshVisibility();
	}
	DetermineCosmeticTags();

	ActivateAnimLayerAndPlayPairedAnim(true, WeaponEquipMontageFP, WeaponEquipMontageTP);

	// IMPORTANT: the blueprint implementation is executed from the original lyraequipmentinstance (K2_OnEquipped)
	Super::OnEquipped();
}

void UMyWeaponInstance::OnUnequipped()
{
	ActivateAnimLayerAndPlayPairedAnim(true, WeaponUnequipMontageFP, WeaponUnequipMontageTP);

	Super::OnUnequipped();
}

TSubclassOf<UAnimInstance> UMyWeaponInstance::PickBestAnimLayer_FP(
	bool bEquipped,
	const FGameplayTagContainer& CosmeticTags
) const
{
	const FLyraAnimLayerSelectionSet& SetToQuery_FP = (bEquipped ? EquippedAnimSet_FP : UnequippedAnimSet_FP);
	return SetToQuery_FP.SelectBestLayer(CosmeticTags);
}

void UMyWeaponInstance::DetermineCosmeticTags()
{
	if (AFPSPlayerCharacter* Character = Cast<AFPSPlayerCharacter>(GetPawn()))
	{
		if (UActorComponent* comp = Character->GetComponentByClass(UMyPawnComp_CharacterParts::StaticClass()))
		{
			CosmeticComponent = Cast<UMyPawnComp_CharacterParts>(comp);
			if (IsValid(CosmeticComponent))
			{
				CosmeticAnimStyleTags = CosmeticComponent->GetCombinedTags(FGameplayTag::RequestGameplayTag("Cosmetic.AnimationStyle"));
			}
		}
	}
}

void UMyWeaponInstance::ActivateAnimLayerAndPlayPairedAnim(bool Equipped, UAnimMontage* FPMontage, UAnimMontage* TPMontage)
{
	AFPSPlayerCharacter* Character = Cast<AFPSPlayerCharacter>(GetPawn());
	if (!CosmeticAnimStyleTags.IsEmpty())
	{
		if (USkeletalMeshComponent* FPMesh = Character->GetFirstPersonMesh())
		{
			if (TSubclassOf<UAnimInstance> ChosenAnimLayer = PickBestAnimLayer_FP(Equipped, CosmeticAnimStyleTags))
			{
				FPMesh->LinkAnimClassLayers(ChosenAnimLayer);
			}
			FPMesh->GetAnimInstance()->Montage_Play(FPMontage);
		}

		USkeletalMeshComponent* TPMesh = Character->GetMesh();
		if (TSubclassOf<UAnimInstance> ChosenAnimLayer = PickBestAnimLayer(Equipped, CosmeticAnimStyleTags))
		{
			TPMesh->LinkAnimClassLayers(ChosenAnimLayer);
		}
		TPMesh->GetAnimInstance()->Montage_Play(TPMontage);
	}
}
