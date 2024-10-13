// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "MyEquipmentManagerComponent.generated.h"

class UFPSEquipmentDefinition;
class UFPSRangedWeaponInstance;

USTRUCT(BlueprintType)
struct FMyEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FMyEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FMyEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:

	UFPSRangedWeaponInstance* AddEntry_V2(TSubclassOf<UFPSEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(ULyraEquipmentInstance* Instance);

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FLyraAppliedEquipmentEntry, FMyEquipmentList>(Entries, DeltaParms, *this);
	}

	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FLyraAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	ULyraAbilitySystemComponent* GetAbilitySystemComponent() const;

private:

	friend UMyEquipmentManagerComponent;
};

template<>
struct TStructOpsTypeTraits<FMyEquipmentList> : public TStructOpsTypeTraitsBase2<FMyEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};

/**
 * 
 */
UCLASS(BlueprintType, Const)
class FPSCORERUNTIME_API UMyEquipmentManagerComponent : public ULyraEquipmentManagerComponent
{
	GENERATED_BODY()
	
public:
	UMyEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	ULyraEquipmentInstance* EquipItem_V2(TSubclassOf<UFPSEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem_V2(ULyraEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ULyraEquipmentInstance* GetFirstInstanceOfType_V2(TSubclassOf<ULyraEquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<ULyraEquipmentInstance*> GetEquipmentInstancesOfType_V2(TSubclassOf<ULyraEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType_V2()
	{
		return (T*)GetFirstInstanceOfType_V2(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FMyEquipmentList EquipmentList_V2;
};
