// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEquipmentManagerComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "Equipment/FPSEquipmentDefinition.h"
#include "Weapons/FPSRangedWeaponInstance.h"
#include "Weapons/FPSRangedWeaponInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

//////////////////////////////////////////////////////////////////////
// FMyAppliedEquipmentEntry

FString FMyAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// FMyEquipmentList

UMyWeaponInstance* FMyEquipmentList::AddEntry_V2(TSubclassOf<UFPSEquipmentDefinition> EquipmentDefinition)
{
	UMyWeaponInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UFPSEquipmentDefinition* EquipmentCDO = GetDefault<UFPSEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<ULyraEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr || !InstanceType->IsChildOf(UMyWeaponInstance::StaticClass()))
	{
		InstanceType = UFPSEquipmentDefinition::StaticClass();
	}

	FMyAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.SetEquipmentDefinition(EquipmentDefinition);
	NewEntry.SetEquipmentInstance(NewObject<UMyWeaponInstance>(OwnerComponent->GetOwner(), InstanceType));  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.GetEquipmentInstance();

	if (ULyraAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const ULyraAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}
	
	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
	Result->SpawnEquipmentActors_FP(EquipmentCDO->ActorsToSpawn_FP);

	MarkItemDirty(NewEntry);

	return Result;
}

void FMyEquipmentList::RemoveEntry(ULyraEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FMyAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (ULyraAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();
			

			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

ULyraAbilitySystemComponent* FMyEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<ULyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

void FMyEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FMyAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
	}
}

void FMyEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FMyAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void FMyEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// 	for (int32 Index : ChangedIndices)
	// 	{
	// 		const FGameplayTagStack& Stack = Stacks[Index];
	// 		TagToCountMap[Stack.Tag] = Stack.StackCount;
	// 	}
}

///////////////////////////////////////////////////////////////////////////////////////// equipment manager component

UMyEquipmentManagerComponent::UMyEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList_V2(this)
{
}

void UMyEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList_V2);
}

ULyraEquipmentInstance* UMyEquipmentManagerComponent::EquipItem_V2(TSubclassOf<UFPSEquipmentDefinition> EquipmentClass)
{
	ULyraEquipmentInstance* Result = nullptr;
	if (EquipmentClass != nullptr)
	{
		Result = EquipmentList_V2.AddEntry_V2(EquipmentClass);
		if (Result != nullptr)
		{
			Result->OnEquipped();

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}
	return Result;
}

void UMyEquipmentManagerComponent::UnequipItem_V2(ULyraEquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		ItemInstance->OnUnequipped();
		EquipmentList_V2.RemoveEntry(ItemInstance);
	}
}

bool UMyEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FMyAppliedEquipmentEntry& Entry : EquipmentList_V2.Entries)
	{
		ULyraEquipmentInstance* Instance = Entry.GetEquipmentInstance();

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UMyEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UMyEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UMyWeaponInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FMyAppliedEquipmentEntry& Entry : EquipmentList_V2.Entries)
	{
		AllEquipmentInstances.Add(Entry.GetEquipmentInstance());
	}

	for (UMyWeaponInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem_V2(EquipInstance);
	}

	Super::UninitializeComponent();
}

void UMyEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing LyraEquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FMyAppliedEquipmentEntry& Entry : EquipmentList_V2.Entries)
		{
			ULyraEquipmentInstance* Instance = Entry.GetEquipmentInstance();

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

UMyWeaponInstance* UMyEquipmentManagerComponent::GetFirstInstanceOfType_V2(TSubclassOf<UMyWeaponInstance> InstanceType)
{
	for (FMyAppliedEquipmentEntry& Entry : EquipmentList_V2.Entries)
	{
		if (UMyWeaponInstance* Instance = Entry.GetEquipmentInstance())
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UMyWeaponInstance*> UMyEquipmentManagerComponent::GetEquipmentInstancesOfType_V2(TSubclassOf<UMyWeaponInstance> InstanceType) const
{
	TArray<UMyWeaponInstance*> Results;
	for (const FMyAppliedEquipmentEntry& Entry : EquipmentList_V2.Entries)
	{
		if (UMyWeaponInstance* Instance = Entry.GetEquipmentInstance())
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}