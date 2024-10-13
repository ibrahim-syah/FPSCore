// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/MyWeaponStateComponent.h"
#include "Equipment/MyEquipmentManagerComponent.h"
#include "Weapons/FPSRangedWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyWeaponStateComponent)

UMyWeaponStateComponent::UMyWeaponStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyWeaponStateComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (UMyEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UMyEquipmentManagerComponent>())
		{
			if (UFPSRangedWeaponInstance* CurrentWeapon = Cast<UFPSRangedWeaponInstance>(EquipmentManager->GetFirstInstanceOfType_V2(UFPSRangedWeaponInstance::StaticClass())))
			{
				CurrentWeapon->Tick(DeltaTime);
			}
		}
	}
}