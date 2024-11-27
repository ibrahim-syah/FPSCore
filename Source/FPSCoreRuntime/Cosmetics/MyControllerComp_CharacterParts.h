// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ControllerComponent.h"

#include "MyControllerComp_CharacterParts.generated.h"

class UChildActorComponent;
class UMyPawnComp_CharacterParts;
struct FMyCharacterPartList;

////////////////////////////////////////////////////////////////////// start of MyCharacterPartTypes

// How should collision be configured on the spawned part actor
UENUM()
enum class EMyCharacterCustomizationCollisionMode : uint8
{
	// Disable collision on spawned character parts
	NoCollision,

	// Leave the collision settings on character parts alone
	UseCollisionFromCharacterPart
};

//////////////////////////////////////////////////////////////////////

// A handle created by adding a character part entry, can be used to remove it later
USTRUCT(BlueprintType)
struct FMyCharacterPartHandle
{
	GENERATED_BODY()

	void Reset()
	{
		PartHandle = INDEX_NONE;
	}

	bool IsValid() const
	{
		return PartHandle != INDEX_NONE;
	}

private:
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	friend FMyCharacterPartList;
};

UENUM()
enum class EMyCharacterPartsMeshType : uint8
{
	// Disable collision on spawned character parts
	Arms1P,
	Legs1P,
	FullBody3P
};

//////////////////////////////////////////////////////////////////////
// A character part request

USTRUCT(BlueprintType)
struct FMyCharacterPart
{
	GENERATED_BODY()

	// The part to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PartClass;

	// The socket to attach the part to (if any)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	// How to handle collision for the primitive components in the part
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMyCharacterCustomizationCollisionMode CollisionMode = EMyCharacterCustomizationCollisionMode::NoCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMyCharacterPartsMeshType MeshType;

	// Compares against another part, ignoring the collision mode
	static bool AreEquivalentParts(const FMyCharacterPart& A, const FMyCharacterPart& B)
	{
		return (A.PartClass == B.PartClass) && (A.SocketName == B.SocketName) && (A.MeshType == B.MeshType);
	}
};

////////////////////////////////////////////////////////////////////////////////// end of MyCharacterPartTypes

class APawn;
class UObject;
struct FFrame;

enum class EMyCharacterPartSource : uint8
{
	Natural,

	NaturalSuppressedViaCheat,

	AppliedViaDeveloperSettingsCheat,

	AppliedViaCheatManager
};

//////////////////////////////////////////////////////////////////////

// A character part requested on a controller component
USTRUCT()
struct FMyControllerCharacterPartEntry
{
	GENERATED_BODY()

	FMyControllerCharacterPartEntry()
	{}

public:
	// The character part being represented
	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FMyCharacterPart Part;

	// The handle if already applied to a pawn
	FMyCharacterPartHandle Handle;

	// The source of this part
	EMyCharacterPartSource Source = EMyCharacterPartSource::Natural;
};

//////////////////////////////////////////////////////////////////////

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class FPSCORERUNTIME_API UMyControllerComp_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()
	
public:
	UMyControllerComp_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	// Adds a character part to the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Cosmetics)
	void AddCharacterPart(const FMyCharacterPart& NewPart);

	// Removes a previously added character part from the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Cosmetics)
	void RemoveCharacterPart(const FMyCharacterPart& PartToRemove);

	// Removes all added character parts, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Cosmetics)
	void RemoveAllCharacterParts();

	// Applies relevant developer settings if in PIE
	void ApplyDeveloperSettings();

protected:
	UPROPERTY(EditAnywhere, Category = Cosmetics)
	TArray<FMyControllerCharacterPartEntry> CharacterParts;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AController* GetOwningController() { return GetController<AController>(); }

private:
	UMyPawnComp_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void AddCharacterPartInternal(const FMyCharacterPart& NewPart, EMyCharacterPartSource Source);

	void AddCheatPart(const FMyCharacterPart& NewPart, bool bSuppressNaturalParts);
	void ClearCheatParts();

	void SetSuppressionOnNaturalParts(bool bSuppressed);

	friend class ULyraCosmeticCheats;
};
