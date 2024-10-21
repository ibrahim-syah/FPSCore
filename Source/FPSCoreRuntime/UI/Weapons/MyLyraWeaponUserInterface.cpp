// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraWeaponUserInterface.h"
#include "Equipment/MyEquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "Weapons/FPSRangedWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLyraWeaponUserInterface)

UMyLyraWeaponUserInterface::UMyLyraWeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UMyEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UMyEquipmentManagerComponent>())
		{
			if (UMyWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UMyWeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					ULyraWeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					RebuildWidgetFromWeapon();
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}