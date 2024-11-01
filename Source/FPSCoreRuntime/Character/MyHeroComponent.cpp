// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyHeroComponent.h"
#include "Equipment/LyraQuickBarComponent.h"
#include "Weapons/FPSRangedWeaponInstance.h"
#include "InputActionValue.h"

namespace LyraHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

UMyHeroComponent::UMyHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	/*if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}*/

	RotArrayX[RotCacheIndex] = Value.X;
	float result = 0.f;
	for (int i = 0; i < MaxRotCache; i++)
	{
		result += RotArrayX[i];
	}
	Pawn->AddControllerYawInput(result / MaxRotCache);

	/*if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}*/

	RotArrayY[RotCacheIndex] = Value.Y;
	result = 0.f;
	for (int i = 0; i < MaxRotCache; i++)
	{
		result += RotArrayY[i];
	}
	Pawn->AddControllerPitchInput(result / MaxRotCache);

	RotCacheIndex++;
	RotCacheIndex %= MaxRotCache;

	AController* PC = Pawn->GetController();
	if (ULyraQuickBarComponent* QuickBarComp = PC->GetComponentByClass<ULyraQuickBarComponent>())
	{
		if (UFPSRangedWeaponInstance* weapon = Cast<UFPSRangedWeaponInstance>(QuickBarComp->GetEquippedItem()))
		{
			weapon->OnLookInput(Value.X, Value.Y);
		}
	}
}

void UMyHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * LyraHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * LyraHero::LookPitchRate * World->GetDeltaSeconds());
	}
}
