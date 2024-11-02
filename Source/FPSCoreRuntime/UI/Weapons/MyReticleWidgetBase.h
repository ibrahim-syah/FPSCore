// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "MyReticleWidgetBase.generated.h"

class ULyraInventoryItemInstance;
class UMyWeaponInstance;
class UObject;
struct FFrame;

/**
 * 
 */
UCLASS(Abstract)
class FPSCORERUNTIME_API UMyReticleWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UMyReticleWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponInitialized();

	UFUNCTION(BlueprintCallable)
	void InitializeFromWeapon(UMyWeaponInstance* InWeapon);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMyWeaponInstance> WeaponInstance;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULyraInventoryItemInstance> InventoryInstance;
};
