// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/MyReticleWidgetBase.h"

#include "Inventory/LyraInventoryItemInstance.h"
#include "Weapons/MyWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyReticleWidgetBase)

UMyReticleWidgetBase::UMyReticleWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyReticleWidgetBase::InitializeFromWeapon(UMyWeaponInstance* InWeapon)
{
	WeaponInstance = InWeapon;
	InventoryInstance = nullptr;
	if (WeaponInstance)
	{
		InventoryInstance = Cast<ULyraInventoryItemInstance>(WeaponInstance->GetInstigator());
	}
	OnWeaponInitialized();
}