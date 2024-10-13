// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "MyControllerComp_CharacterParts.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "MyPawnComp_CharacterParts.generated.h"

class UMyPawnComp_CharacterParts;
namespace EEndPlayReason { enum Type : int; }
struct FMyCharacterPartList;

class UChildActorComponent;
struct FNetDeltaSerializeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMySpawnedCharacterPartsChanged, UMyPawnComp_CharacterParts*, ComponentWithChangedParts);

////////////////////////////////////////////////////////////////////// start of MyCosmeticAnimationTypes

class UAnimInstance;
class UPhysicsAsset;
class USkeletalMesh;

USTRUCT(BlueprintType)
struct FMyAnimLayerSelectionEntry
{
	GENERATED_BODY()

	// Layer to apply if the tag matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> TPFullBody_Layer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> FPArms_Layer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> FPLegs_Layer;

	// Cosmetic tags required (all of these must be present to be considered a match)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FMyAnimLayerSelectionSet
{
	GENERATED_BODY()

	// List of layer rules to apply, first one that matches will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = Layer))
	TArray<FMyAnimLayerSelectionEntry> LayerRules;

	// The layer to use if none of the LayerRules matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Default_TPFullBody_Layer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Default_FPArms_Layer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Default_FPLegs_Layer;

	// Choose the best layer given the rules
	TSubclassOf<UAnimInstance> SelectBestTPFullBodyLayer(const FGameplayTagContainer& CosmeticTags) const;
	TSubclassOf<UAnimInstance> SelectBestFPArmsLayer(const FGameplayTagContainer& CosmeticTags) const;
	TSubclassOf<UAnimInstance> SelectBestFPLegsLayer(const FGameplayTagContainer& CosmeticTags) const;
};

//////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FMyAnimBodyStyleSelectionEntry
{
	GENERATED_BODY()

	// Layer to apply if the tag matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	// Cosmetic tags required (all of these must be present to be considered a match)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FMyAnimBodyStyleSelectionSet
{
	GENERATED_BODY()

	// List of layer rules to apply, first one that matches will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = Mesh))
	TArray<FMyAnimBodyStyleSelectionEntry> MeshRules;

	// The layer to use if none of the LayerRules matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	// If set, ensures this physics asset is always used
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;

	// Choose the best body style skeletal mesh given the rules
	USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;
};

////////////////////////////////////////////////////////////////////// end of MyCosmeticAnimationTypes

// A single applied character part
USTRUCT()
struct FMyAppliedCharacterPartEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FMyAppliedCharacterPartEntry()
	{}

	FString GetDebugString() const;

private:
	friend FMyCharacterPartList;
	friend UMyPawnComp_CharacterParts;

private:
	// The character part being represented
	UPROPERTY()
	FMyCharacterPart Part;

	// Handle index we returned to the user (server only)
	UPROPERTY(NotReplicated)
	int32 PartHandle = INDEX_NONE;

	// The spawned actor instance (client only)
	UPROPERTY(NotReplicated)
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

//////////////////////////////////////////////////////////////////////

// Replicated list of applied character parts
USTRUCT(BlueprintType)
struct FMyCharacterPartList : public FFastArraySerializer
{
	GENERATED_BODY()

	FMyCharacterPartList()
		: OwnerComponent(nullptr)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FMyAppliedCharacterPartEntry, FMyCharacterPartList>(Entries, DeltaParms, *this);
	}

	FMyCharacterPartHandle AddEntry(FMyCharacterPart NewPart);
	void RemoveEntry(FMyCharacterPartHandle Handle);
	void ClearAllEntries(bool bBroadcastChangeDelegate);

	FGameplayTagContainer CollectCombinedTags() const;

	void SetOwnerComponent(UMyPawnComp_CharacterParts* InOwnerComponent)
	{
		OwnerComponent = InOwnerComponent;
	}

private:
	friend UMyPawnComp_CharacterParts;

	bool SpawnActorForEntry(FMyAppliedCharacterPartEntry& Entry);
	bool DestroyActorForEntry(FMyAppliedCharacterPartEntry& Entry);

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FMyAppliedCharacterPartEntry> Entries;

	// The component that contains this list
	UPROPERTY(NotReplicated)
	TObjectPtr<UMyPawnComp_CharacterParts> OwnerComponent;

	// Upcounter for handles
	int32 PartHandleCounter = 0;
};

template<>
struct TStructOpsTypeTraits<FMyCharacterPartList> : public TStructOpsTypeTraitsBase2<FMyCharacterPartList>
{
	enum { WithNetDeltaSerializer = true };
};

//////////////////////////////////////////////////////////////////////

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class FPSCORERUNTIME_API UMyPawnComp_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UMyPawnComp_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	//~End of UActorComponent interface

	// Adds a character part to the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Cosmetics)
	FMyCharacterPartHandle AddCharacterPart(const FMyCharacterPart& NewPart);

	// Removes a previously added character part from the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Cosmetics)
	void RemoveCharacterPart(FMyCharacterPartHandle Handle);

	// Removes all added character parts, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Cosmetics)
	void RemoveAllCharacterParts();

	// Gets the list of all spawned character parts from this component
	UFUNCTION(BlueprintCallable, BlueprintPure = false, BlueprintCosmetic, Category = Cosmetics)
	TArray<AActor*> GetCharacterPartActors() const;

	// If the parent actor is derived from ACharacter, returns the Mesh component, otherwise nullptr
	USkeletalMeshComponent* GetParentMeshComponent() const;

	// If the parent actor is derived from FPSPlayerCharacter, returns the SkeletalMesh component depending on mesh type
	USkeletalMeshComponent* GetCosmeticMeshComponent(FMyCharacterPart& Part) const;

	// Returns the scene component to attach the spawned actors to
	// If the parent actor is derived from ACharacter, we'll use the Mesh component, otherwise the root component
	USceneComponent* GetSceneComponentToAttachTo(FMyCharacterPart& Part) const;

	// Returns the set of combined gameplay tags from attached character parts, optionally filtered to only tags that start with the specified root
	UFUNCTION(BlueprintCallable, BlueprintPure = false, BlueprintCosmetic, Category = Cosmetics)
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;

	void BroadcastChanged();

public:
	// Delegate that will be called when the list of spawned character parts has changed
	UPROPERTY(BlueprintAssignable, Category = Cosmetics, BlueprintCallable)
	FMySpawnedCharacterPartsChanged OnCharacterPartsChanged;

private:
	// List of character parts
	UPROPERTY(Replicated, Transient)
	FMyCharacterPartList CharacterPartList;

	// Rules for how to pick a body style mesh for animation to play on, based on character part cosmetics tags
	UPROPERTY(EditAnywhere, Category = Cosmetics)
	FMyAnimBodyStyleSelectionSet BodyMeshes;
};
