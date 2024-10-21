// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuickBarComponent.h"
#include "FPSEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "MyEquipmentManagerComponent.h"

UMyQuickBarComponent::UMyQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyQuickBarComponent::EquipItemInSlot()
{
	check(GetSlots().IsValidIndex(GetActiveSlotIndex()));
	check(EquippedItem == nullptr);

	if (ULyraInventoryItemInstance* SlotItem = GetActiveSlotItem())
	{
		if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
		{
			TSubclassOf<ULyraEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
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
