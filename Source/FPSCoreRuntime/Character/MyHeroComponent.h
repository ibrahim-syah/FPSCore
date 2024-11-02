// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraHeroComponent.h"
#include "MyHeroComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class FPSCORERUNTIME_API UMyHeroComponent : public ULyraHeroComponent
{
	GENERATED_BODY()
	
public:
	UMyHeroComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Input_LookMouse(const FInputActionValue& InputActionValue) override;
	virtual void Input_LookStick(const FInputActionValue& InputActionValue) override;

	int64 MaxRotCache = 3;
	float RotArrayX[3];
	float RotArrayY[3];
	int64 RotCacheIndex = 0;
};
