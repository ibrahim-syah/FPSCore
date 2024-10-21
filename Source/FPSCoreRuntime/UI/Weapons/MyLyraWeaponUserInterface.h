// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Weapons/LyraWeaponUserInterface.h"
#include "MyLyraWeaponUserInterface.generated.h"

class UMyWeaponInstance;

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UMyLyraWeaponUserInterface : public ULyraWeaponUserInterface
{
	GENERATED_BODY()
	
public:
	UMyLyraWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
