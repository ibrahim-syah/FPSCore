// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/LyraInventoryItemDefinition.h"
#include "MyInventoryFragment_ReticleConf.generated.h"

class UMyReticleWidgetBase;

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UMyInventoryFragment_ReticleConf : public ULyraInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Reticle)
	TArray<TSubclassOf<UMyReticleWidgetBase>> ReticleWidgets;
};
