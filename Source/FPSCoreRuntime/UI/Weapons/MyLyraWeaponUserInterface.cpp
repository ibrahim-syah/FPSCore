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
			if (UMyWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType_V2<UMyWeaponInstance>())
			{
				if (NewInstance != CurrentInstance_V2 && NewInstance->GetInstigator() != nullptr)
				{
					UMyWeaponInstance* OldWeapon = CurrentInstance_V2;
					CurrentInstance_V2 = NewInstance;
					RebuildWidgetFromWeapon();
					//OnWeaponChanged_V2(OldWeapon, CurrentInstance_V2);
					OnWeaponChanged(OldWeapon, CurrentInstance_V2);
				}
			}
		}
	}
}