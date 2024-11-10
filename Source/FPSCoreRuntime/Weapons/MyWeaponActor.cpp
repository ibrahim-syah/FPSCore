// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/MyWeaponActor.h"

// Sets default values
AMyWeaponActor::AMyWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->CastShadow = true;
	WeaponMesh->bCastHiddenShadow = true;
}

// Called when the game starts or when spawned
void AMyWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (bIsFP)
	{
		WeaponMesh->SetOnlyOwnerSee(true);
		WeaponMesh->SetCastShadow(false);
		WeaponMesh->SetBoundsScale(2.f);
	}
}

