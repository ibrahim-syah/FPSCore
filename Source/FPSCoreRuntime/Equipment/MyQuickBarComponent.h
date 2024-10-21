// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/LyraQuickBarComponent.h"
#include "MyQuickBarComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class FPSCORERUNTIME_API UMyQuickBarComponent : public ULyraQuickBarComponent
{
	GENERATED_BODY()

public:
	UMyQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void UnequipItemInSlot() override;
	virtual void EquipItemInSlot() override;

	
};
