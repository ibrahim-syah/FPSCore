// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuickBarComponent.h"
#include "FPSEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "Inventory/MyInventoryFrag_EquippableItem.h"
#include "MyEquipmentManagerComponent.h"

UMyQuickBarComponent::UMyQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyQuickBarComponent::SetActiveSlotIndex_V2_Implementation(int32 NewIndex)
{
	if (GetSlots().IsValidIndex(NewIndex) && (GetActiveSlotIndex() != NewIndex))
	{
		UnequipItemInSlot();

		ActiveSlotIndex = NewIndex;

		EquipItemInSlot();

		OnRep_ActiveSlotIndex();
	}
}

void UMyQuickBarComponent::CycleActiveSlotForward_V2()
{
	if (GetSlots().Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? GetSlots().Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % GetSlots().Num();
		if (GetSlots()[NewIndex] != nullptr)
		{
			SetActiveSlotIndex_V2(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UMyQuickBarComponent::CycleActiveSlotBackward_V2()
{
	if (GetSlots().Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? GetSlots().Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + GetSlots().Num()) % GetSlots().Num();
		if (GetSlots()[NewIndex] != nullptr)
		{
			SetActiveSlotIndex_V2(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UMyQuickBarComponent::EquipItemInSlot()
{
	check(GetSlots().IsValidIndex(GetActiveSlotIndex()));
	check(EquippedItem == nullptr);

	if (ULyraInventoryItemInstance* SlotItem = GetActiveSlotItem())
	{
		if (const UMyInventoryFrag_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UMyInventoryFrag_EquippableItem>())
		{
			TSubclassOf<UFPSEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition_V2;
			if (EquipDef != nullptr)
			{
				if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					if (UMyEquipmentManagerComponent* CastedEquipmentManager = Cast<UMyEquipmentManagerComponent>(EquipmentManager))
					{
						EquippedItem = CastedEquipmentManager->EquipItem(EquipDef);
						if (EquippedItem != nullptr)
						{
							EquippedItem->SetInstigator(SlotItem);
						}
					}
				}
			}
		}
	}
}

void UMyQuickBarComponent::UnequipItemInSlot()
{
	if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (UMyEquipmentManagerComponent* CastedEquipmentManager = Cast<UMyEquipmentManagerComponent>(EquipmentManager))
		{
			if (EquippedItem != nullptr)
			{
				CastedEquipmentManager->UnequipItem(EquippedItem);
				EquippedItem = nullptr;
			}
		}
	}
}
