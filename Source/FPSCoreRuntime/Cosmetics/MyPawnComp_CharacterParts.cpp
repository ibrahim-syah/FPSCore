// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawnComp_CharacterParts.h"
#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameplayTagAssetInterface.h"
#include "Character/FPSPlayerCharacter.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyPawnComp_CharacterParts)

USkeletalMesh* FMyAnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
	for (const FMyAnimBodyStyleSelectionEntry& Rule : MeshRules)
	{
		if ((Rule.Mesh != nullptr) && CosmeticTags.HasAll(Rule.RequiredTags))
		{
			return Rule.Mesh;
		}
	}

	return DefaultMesh;
}

//////////////////////////////////////////////////////////////////////

FString FMyAppliedCharacterPartEntry::GetDebugString() const
{
	return FString::Printf(TEXT("(PartClass: %s, Socket: %s, Instance: %s)"), *GetPathNameSafe(Part.PartClass), *Part.SocketName.ToString(), *GetPathNameSafe(SpawnedComponent));
}

//////////////////////////////////////////////////////////////////////

void FMyCharacterPartList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	bool bDestroyedAnyActors = false;
	for (int32 Index : RemovedIndices)
	{
		FMyAppliedCharacterPartEntry& Entry = Entries[Index];
		bDestroyedAnyActors |= DestroyActorForEntry(Entry);
	}

	if (bDestroyedAnyActors && ensure(OwnerComponent))
	{
		OwnerComponent->BroadcastChanged();
	}
}

void FMyCharacterPartList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	bool bCreatedAnyActors = false;
	for (int32 Index : AddedIndices)
	{
		FMyAppliedCharacterPartEntry& Entry = Entries[Index];
		bCreatedAnyActors |= SpawnActorForEntry(Entry);
	}

	if (bCreatedAnyActors && ensure(OwnerComponent))
	{
		OwnerComponent->BroadcastChanged();
	}
}

void FMyCharacterPartList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	bool bChangedAnyActors = false;

	// We don't support dealing with propagating changes, just destroy and recreate
	for (int32 Index : ChangedIndices)
	{
		FMyAppliedCharacterPartEntry& Entry = Entries[Index];

		bChangedAnyActors |= DestroyActorForEntry(Entry);
		bChangedAnyActors |= SpawnActorForEntry(Entry);
	}

	if (bChangedAnyActors && ensure(OwnerComponent))
	{
		OwnerComponent->BroadcastChanged();
	}
}

FMyCharacterPartHandle FMyCharacterPartList::AddEntry(FMyCharacterPart NewPart)
{
	FMyCharacterPartHandle Result;
	Result.PartHandle = PartHandleCounter++;

	if (ensure(OwnerComponent && OwnerComponent->GetOwner() && OwnerComponent->GetOwner()->HasAuthority()))
	{
		FMyAppliedCharacterPartEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Part = NewPart;
		NewEntry.PartHandle = Result.PartHandle;

		if (SpawnActorForEntry(NewEntry))
		{
			OwnerComponent->BroadcastChanged();
		}

		MarkItemDirty(NewEntry);
	}

	return Result;
}

void FMyCharacterPartList::RemoveEntry(FMyCharacterPartHandle Handle)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FMyAppliedCharacterPartEntry& Entry = *EntryIt;
		if (Entry.PartHandle == Handle.PartHandle)
		{
			const bool bDestroyedActor = DestroyActorForEntry(Entry);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();

			if (bDestroyedActor && ensure(OwnerComponent))
			{
				OwnerComponent->BroadcastChanged();
			}

			break;
		}
	}
}

void FMyCharacterPartList::ClearAllEntries(bool bBroadcastChangeDelegate)
{
	bool bDestroyedAnyActors = false;
	for (FMyAppliedCharacterPartEntry& Entry : Entries)
	{
		bDestroyedAnyActors |= DestroyActorForEntry(Entry);
	}
	Entries.Reset();
	MarkArrayDirty();

	if (bDestroyedAnyActors && bBroadcastChangeDelegate && ensure(OwnerComponent))
	{
		OwnerComponent->BroadcastChanged();
	}
}

FGameplayTagContainer FMyCharacterPartList::CollectCombinedTags() const
{
	FGameplayTagContainer Result;

	for (const FMyAppliedCharacterPartEntry& Entry : Entries)
	{
		if (Entry.SpawnedComponent != nullptr)
		{
			if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Entry.SpawnedComponent->GetChildActor()))
			{
				TagInterface->GetOwnedGameplayTags(/*inout*/ Result);
			}
		}
	}

	return Result;
}

bool FMyCharacterPartList::SpawnActorForEntry(FMyAppliedCharacterPartEntry& Entry)
{
	bool bCreatedAnyActors = false;

	if (ensure(OwnerComponent) && !OwnerComponent->IsNetMode(NM_DedicatedServer))
	{
		if (Entry.Part.PartClass != nullptr)
		{
			UWorld* World = OwnerComponent->GetWorld();

			if (USceneComponent* ComponentToAttachTo = OwnerComponent->GetSceneComponentToAttachTo(Entry.Part))
			{
				const FTransform SpawnTransform = ComponentToAttachTo->GetSocketTransform(Entry.Part.SocketName);

				UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(OwnerComponent->GetOwner());

				PartComponent->SetupAttachment(ComponentToAttachTo, Entry.Part.SocketName);
				PartComponent->SetChildActorClass(Entry.Part.PartClass);
				PartComponent->RegisterComponent();

				if (AActor* SpawnedActor = PartComponent->GetChildActor())
				{
					switch (Entry.Part.CollisionMode)
					{
					case EMyCharacterCustomizationCollisionMode::UseCollisionFromCharacterPart:
						// Do nothing
						break;

					case EMyCharacterCustomizationCollisionMode::NoCollision:
						SpawnedActor->SetActorEnableCollision(false);
						break;
					}

					// Set up a direct tick dependency to work around the child actor component not providing one
					if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
					{
						SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
					}
				}

				Entry.SpawnedComponent = PartComponent;
				bCreatedAnyActors = true;
			}
		}
	}

	return bCreatedAnyActors;
}

bool FMyCharacterPartList::DestroyActorForEntry(FMyAppliedCharacterPartEntry& Entry)
{
	bool bDestroyedAnyActors = false;

	if (Entry.SpawnedComponent != nullptr)
	{
		Entry.SpawnedComponent->DestroyComponent();
		Entry.SpawnedComponent = nullptr;
		bDestroyedAnyActors = true;
	}

	return bDestroyedAnyActors;
}

//////////////////////////////////////////////////////////////////////

UMyPawnComp_CharacterParts::UMyPawnComp_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UMyPawnComp_CharacterParts::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterPartList);
}

FMyCharacterPartHandle UMyPawnComp_CharacterParts::AddCharacterPart(const FMyCharacterPart& NewPart)
{
	return CharacterPartList.AddEntry(NewPart);
}

void UMyPawnComp_CharacterParts::RemoveCharacterPart(FMyCharacterPartHandle Handle)
{
	CharacterPartList.RemoveEntry(Handle);
}

void UMyPawnComp_CharacterParts::RemoveAllCharacterParts()
{
	CharacterPartList.ClearAllEntries(/*bBroadcastChangeDelegate=*/ true);
}

void UMyPawnComp_CharacterParts::BeginPlay()
{
	Super::BeginPlay();
}

void UMyPawnComp_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CharacterPartList.ClearAllEntries(/*bBroadcastChangeDelegate=*/ false);

	Super::EndPlay(EndPlayReason);
}

void UMyPawnComp_CharacterParts::OnRegister()
{
	Super::OnRegister();

	if (!IsTemplate())
	{
		CharacterPartList.SetOwnerComponent(this);
	}
}

TArray<AActor*> UMyPawnComp_CharacterParts::GetCharacterPartActors() const
{
	TArray<AActor*> Result;
	Result.Reserve(CharacterPartList.Entries.Num());

	for (const FMyAppliedCharacterPartEntry& Entry : CharacterPartList.Entries)
	{
		if (UChildActorComponent* PartComponent = Entry.SpawnedComponent)
		{
			if (AActor* SpawnedActor = PartComponent->GetChildActor())
			{
				Result.Add(SpawnedActor);
			}
		}
	}

	return Result;
}

USkeletalMeshComponent* UMyPawnComp_CharacterParts::GetParentMeshComponent() const
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor))
		{
			if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetMesh())
			{
				return MeshComponent;
			}
		}
	}

	return nullptr;
}

USkeletalMeshComponent* UMyPawnComp_CharacterParts::GetCosmeticMeshComponent(FMyCharacterPart& Part) const
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (AFPSPlayerCharacter* OwningCharacter = Cast<AFPSPlayerCharacter>(OwnerActor))
		{
			switch (Part.MeshType)
			{
			case EMyCharacterPartsMeshType::FullBody3P:
				if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetMesh())
				{
					return MeshComponent;
				}
			case EMyCharacterPartsMeshType::Arms1P:
				if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetFirstPersonMesh())
				{
					return MeshComponent;
				}
			case EMyCharacterPartsMeshType::Legs1P:
				if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetFirstPersonLegMesh())
				{
					return MeshComponent;
				}
			default:
				break;
			}
		}
	}

	return nullptr;
}

USceneComponent* UMyPawnComp_CharacterParts::GetSceneComponentToAttachTo(FMyCharacterPart& Part) const
{
	if (USkeletalMeshComponent* MeshComponent = GetCosmeticMeshComponent(Part))
	{
		return MeshComponent;
	}
	else if (AActor* OwnerActor = GetOwner())
	{
		return OwnerActor->GetRootComponent();
	}
	else
	{
		return nullptr;
	}
}

FGameplayTagContainer UMyPawnComp_CharacterParts::GetCombinedTags(FGameplayTag RequiredPrefix) const
{
	FGameplayTagContainer Result = CharacterPartList.CollectCombinedTags();
	if (RequiredPrefix.IsValid())
	{
		return Result.Filter(FGameplayTagContainer(RequiredPrefix));
	}
	else
	{
		return Result;
	}
}

void UMyPawnComp_CharacterParts::BroadcastChanged()
{
	const bool bReinitPose = true;

	// Check to see if the body type has changed
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		// Determine the mesh to use based on cosmetic part tags
		const FGameplayTagContainer MergedTags = GetCombinedTags(FGameplayTag());
		USkeletalMesh* DesiredMesh = BodyMeshes.SelectBestBodyStyle(MergedTags);

		// Apply the desired mesh (this call is a no-op if the mesh hasn't changed)
		MeshComponent->SetSkeletalMesh(DesiredMesh, /*bReinitPose=*/ bReinitPose);

		// Apply the desired physics asset if there's a forced override independent of the one from the mesh
		if (UPhysicsAsset* PhysicsAsset = BodyMeshes.ForcedPhysicsAsset)
		{
			MeshComponent->SetPhysicsAsset(PhysicsAsset, /*bForceReInit=*/ bReinitPose);
		}
	}

	// Let observers know, e.g., if they need to apply team coloring or similar
	OnCharacterPartsChanged.Broadcast(this);
}