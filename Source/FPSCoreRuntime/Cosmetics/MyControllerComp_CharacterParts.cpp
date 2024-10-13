// Fill out your copyright notice in the Description page of Project Settings.


#include "MyControllerComp_CharacterParts.h"
#include "MyPawnComp_CharacterParts.h"
#include "GameFramework/CheatManagerDefines.h"
#include "MyCosmeticDeveloperSettings.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyControllerComp_CharacterParts)

//////////////////////////////////////////////////////////////////////

UMyControllerComp_CharacterParts::UMyControllerComp_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyControllerComp_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	// Listen for pawn possession changed events
	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

			if (APawn* ControlledPawn = GetPawn<APawn>())
			{
				OnPossessedPawnChanged(nullptr, ControlledPawn);
			}
		}

		ApplyDeveloperSettings();
	}
}

void UMyControllerComp_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
}

UMyPawnComp_CharacterParts* UMyControllerComp_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		return ControlledPawn->FindComponentByClass<UMyPawnComp_CharacterParts>();
	}
	return nullptr;
}

void UMyControllerComp_CharacterParts::AddCharacterPart(const FMyCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart, EMyCharacterPartSource::Natural);
}

void UMyControllerComp_CharacterParts::AddCharacterPartInternal(const FMyCharacterPart& NewPart, EMyCharacterPartSource Source)
{
	FMyControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;
	NewEntry.Source = Source;

	if (UMyPawnComp_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		if (NewEntry.Source != EMyCharacterPartSource::NaturalSuppressedViaCheat)
		{
			NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
		}
	}

}

void UMyControllerComp_CharacterParts::RemoveCharacterPart(const FMyCharacterPart& PartToRemove)
{
	for (auto EntryIt = CharacterParts.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (FMyCharacterPart::AreEquivalentParts(EntryIt->Part, PartToRemove))
		{
			if (UMyPawnComp_CharacterParts* PawnCustomizer = GetPawnCustomizer())
			{
				PawnCustomizer->RemoveCharacterPart(EntryIt->Handle);
			}

			EntryIt.RemoveCurrent();
			break;
		}
	}
}

void UMyControllerComp_CharacterParts::RemoveAllCharacterParts()
{
	if (UMyPawnComp_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FMyControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}

	CharacterParts.Reset();
}

void UMyControllerComp_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Remove from the old pawn
	if (UMyPawnComp_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UMyPawnComp_CharacterParts>() : nullptr)
	{
		for (FMyControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// Apply to the new pawn
	if (UMyPawnComp_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UMyPawnComp_CharacterParts>() : nullptr)
	{
		for (FMyControllerCharacterPartEntry& Entry : CharacterParts)
		{
			// Don't readd if it's already there, this can get called with a null oldpawn
			if (!Entry.Handle.IsValid() && Entry.Source != EMyCharacterPartSource::NaturalSuppressedViaCheat)
			{
				Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
			}
		}
	}
}

void UMyControllerComp_CharacterParts::ApplyDeveloperSettings()
{
#if UE_WITH_CHEAT_MANAGER
	const UMyCosmeticDeveloperSettings* Settings = GetDefault<UMyCosmeticDeveloperSettings>();

	// Suppress or unsuppress natural parts if needed
	const bool bSuppressNaturalParts = (Settings->CheatMode == EMyCosmeticCheatMode::ReplaceParts) && (Settings->CheatCosmeticCharacterParts.Num() > 0);
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);

	// Remove anything added by developer settings and re-add it
	UMyPawnComp_CharacterParts* PawnCustomizer = GetPawnCustomizer();
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == EMyCharacterPartSource::AppliedViaDeveloperSettingsCheat)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	// Add new parts
	for (const FMyCharacterPart& PartDesc : Settings->CheatCosmeticCharacterParts)
	{
		AddCharacterPartInternal(PartDesc, EMyCharacterPartSource::AppliedViaDeveloperSettingsCheat);
	}
#endif
}


void UMyControllerComp_CharacterParts::AddCheatPart(const FMyCharacterPart& NewPart, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);
	AddCharacterPartInternal(NewPart, EMyCharacterPartSource::AppliedViaCheatManager);
#endif
}

void UMyControllerComp_CharacterParts::ClearCheatParts()
{
#if UE_WITH_CHEAT_MANAGER
	UMyPawnComp_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	// Remove anything added by cheat manager cheats
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == EMyCharacterPartSource::AppliedViaCheatManager)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	ApplyDeveloperSettings();
#endif
}

void UMyControllerComp_CharacterParts::SetSuppressionOnNaturalParts(bool bSuppressed)
{
#if UE_WITH_CHEAT_MANAGER
	UMyPawnComp_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	for (FMyControllerCharacterPartEntry& Entry : CharacterParts)
	{
		if ((Entry.Source == EMyCharacterPartSource::Natural) && bSuppressed)
		{
			// Suppress
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(Entry.Handle);
				Entry.Handle.Reset();
			}
			Entry.Source = EMyCharacterPartSource::NaturalSuppressedViaCheat;
		}
		else if ((Entry.Source == EMyCharacterPartSource::NaturalSuppressedViaCheat) && !bSuppressed)
		{
			// Unsuppress
			if (PawnCustomizer != nullptr)
			{
				Entry.Handle = PawnCustomizer->AddCharacterPart(Entry.Part);
			}
			Entry.Source = EMyCharacterPartSource::Natural;
		}
	}
#endif
}