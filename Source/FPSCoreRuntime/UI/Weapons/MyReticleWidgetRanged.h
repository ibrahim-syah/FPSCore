// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Weapons/MyReticleWidgetBase.h"
#include "MyReticleWidgetRanged.generated.h"

/**
 * 
 */
UCLASS()
class FPSCORERUNTIME_API UMyReticleWidgetRanged : public UMyReticleWidgetBase
{
	GENERATED_BODY()
	
public:
	UMyReticleWidgetRanged(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Returns the current weapon's diametrical spread angle, in degrees */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float ComputeSpreadAngle() const;

	/** Returns the current weapon's maximum spread radius in screenspace units (pixels) */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float ComputeMaxScreenspaceSpreadRadius() const;

	/**
	 * Returns true if the current weapon is at 'first shot accuracy'
	 * (the weapon allows it and it is at min spread)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasFirstShotAccuracy() const;
};
