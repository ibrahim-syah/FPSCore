// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "MyInventoryFrag_EquippableItem.generated.h"

class UFPSEquipmentDefinition;

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UMyInventoryFrag_EquippableItem : public UInventoryFragment_EquippableItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = Lyra)
	TSubclassOf<UFPSEquipmentDefinition> EquipmentDefinition_V2;
};
