// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/LyraWeaponStateComponent.h"
#include "MyWeaponStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UMyWeaponStateComponent : public ULyraWeaponStateComponent
{
	GENERATED_BODY()
	
public:
	UMyWeaponStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
