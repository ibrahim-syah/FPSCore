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
			// TODO: figure out how to make the tick works for all weapon type, not just ranged weapon
			// maybe make all weapon to be this weapon type? so all weapon extend in blueprint while in cpp it's this one weapon type
			// this makes it all weapon can use the reticle and making another gameplay ability is easier. we only need to figure out which one is 
			// shared in cpp and which ones are extended in bp to make them all work together
			// man i hate oop
			if (UFPSRangedWeaponInstance* CurrentWeapon = Cast<UFPSRangedWeaponInstance>(EquipmentManager->GetFirstInstanceOfType_V2(UFPSRangedWeaponInstance::StaticClass())))
			{
				CurrentWeapon->Tick(DeltaTime);
			}
		}
	}
}