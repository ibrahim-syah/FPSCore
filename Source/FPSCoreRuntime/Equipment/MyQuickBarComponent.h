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

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Lyra")
	void SetActiveSlotIndex_V2(int32 NewIndex);

	UFUNCTION(BlueprintCallable, Category = "Lyra")
	void CycleActiveSlotForward_V2();

	UFUNCTION(BlueprintCallable, Category = "Lyra")
	void CycleActiveSlotBackward_V2();

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	
};
