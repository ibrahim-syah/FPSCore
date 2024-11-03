// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/MyWeaponInstance.h"
#include "MyMeleeWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UMyMeleeWeaponInstance : public UMyWeaponInstance
{
	GENERATED_BODY()
	
public:
	UMyMeleeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
