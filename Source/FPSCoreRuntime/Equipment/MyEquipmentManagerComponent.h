// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "MyEquipmentManagerComponent.generated.h"

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

	ULyraEquipmentInstance* AddEntry_V2(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition);
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

	virtual ULyraEquipmentInstance* EquipItem(TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition) override;

	virtual void UnequipItem(ULyraEquipmentInstance* ItemInstance) override;

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
	virtual ULyraEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<ULyraEquipmentInstance> InstanceType) override;

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	virtual TArray<ULyraEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<ULyraEquipmentInstance> InstanceType) const override;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FMyEquipmentList EquipmentList_V2;
};
