// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyWeaponActor.generated.h"

UCLASS()
class FPSCORERUNTIME_API AMyWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyWeaponActor();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsFP;
};
