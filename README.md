# Lyra First Person Shooter
This is my attempt at implementing first person character based on Lyra. This repo is a game feature plugin that only depends on the base Lyra classes and assets (no dependencies on ShooterCore). [Here](https://youtu.be/F8nx2BPH0cs) is a quick demo.

This setup is inspired by [philspaz](https://github.com/philspaz/AfootLyraExtension) and [Octahedron](https://www.youtube.com/playlist?list=PLxYB4IVFm8q6tf3boC1Nm4A8AB2RMCpXn)

## Prerequisites
- Unreal Engine 5.4
- Unmodified Lyra Project

## Base Class Changes
I tried my best to make minimal changes to the base Lyra classes, but changes here and there are inevitable and some classes are more trouble than its worth to extend.

### ALyraCharacter -> LyraGame/Character/LyraCharacter.h
- export LYRAGAME_API for ALyraCharacter
- add a protected function that returns the camera component:
    ```cpp
    FORCEINLINE ULyraCameraComponent* GetCameraComponent() { return CameraComponent; }
    ```

### ULyraDevelopmentStatics -> LyraGame/System/LyraDevelopmentStatics.h
- export LYRAGAME_API for ULyraDevelopmentStatics 

### ULyraRangedWeaponInstance -> LyraGame/Weapons/LyraRangedWeaponInstance.h
- export LYRAGAME_API for ULyraRangedWeaponInstance

### ULyraEquipmentDefinition -> LyraGaame/Equipment/LyraEquipmentDefinition.h
- export LYRAGAME_API for ULyraEquipmentDefinition

### ULyraWeaponInstance -> LyraGame/Weapons/LyraWeaponInstance.h
- export LYRAGAME_API for ULyraWeaponInstance 

### ULyraEquipmentInstance -> LyraGame/Equipment/LyraEquipmentInstance.h
- export LYRAGAME_API for ULyraEquipmentInstance 
- Turn SpawnedActors into a protected variable

### ULyraEquipmentManagerComponent -> LyraGame/Equipment/LyraEquipmentManagerComponent.h
- export LYRAGAME_API for FLyraAppliedEquipmentEntry
- make changes to FLyraAppliedEquipmentEntry:
	- make Instance public:
        ```cpp
        UPROPERTY()
        TObjectPtr<ULyraEquipmentInstance> Instance = nullptr;
        ```
	- make GrantedHandles public:
        ```cpp
        // Authority-only list of granted handles
        UPROPERTY(NotReplicated)
        FLyraAbilitySet_GrantedHandles GrantedHandles;
        ```
	- add these public functions:
        ```cpp
        TSubclassOf<ULyraEquipmentDefinition> GetEquipmentDefinition() const { return EquipmentDefinition; }
        void SetEquipmentDefinition(TSubclassOf<ULyraEquipmentDefinition> InEquipmentDef) { EquipmentDefinition = InEquipmentDef; }

        TObjectPtr<ULyraEquipmentInstance> GetEquipmentInstance() const { return Instance; }
        void SetEquipmentInstance(TObjectPtr<ULyraEquipmentInstance> InInstance) { Instance = InInstance; }
        ```
- export LYRAGAME_API for ULyraEquipmentManagerComponent

### ULyraInventoryItemInstance -> LyraGame/Inventory/LyraInventoryItemInstance.h
- export LYRAGAME_API for ULyraInventoryItemInstance 

### UInventoryFragment_EquippableItem -> LyraGame/Inventory/InventoryFragment_EquippableItem.h
- export LYRAGAME_API for UInventoryFragment_EquippableItem 

### ULyraAbilitySet -> LyraGame/AbilitySystem/LyraAbilitySet.h
- export LYRAGAME_API for ULyraAbilitySet
- export LYRAGAME_API for FLyraAbilitySet_GrantedHandles

### ULyraQuickBarComponent-> LyraGame/Equipment/LyraQuickBarComponent.h
- export LYRAGAME_API for ULyraQuickBarComponent-
make these changes:
- turn ActiveSlotIndex, EquippedItem, and FindEquipmentManager into protected variables and method:
	```cpp
	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;
	
	UPROPERTY()
	TObjectPtr<ULyraEquipmentInstance> EquippedItem;

	ULyraEquipmentManagerComponent* FindEquipmentManager() const;
	```

### ULyraWeaponUserInterface -> LyraGame/UI/Weapons/LyraWeaponUserInterface.h
- export LYRAGAME_API for ULyraWeaponUserInterface
- turn RebuildWidgetFromWeapon() into a protected method

### ULyraCameraMode -> LyraGame/Camera/LyraCameraMode.h
- create this new protected property:
	```cpp
	UPROPERTY(EditDefaultsOnly, Category = "View")
	bool bIsOverrideFOV = false;
	```
- create these public functions
	```cpp
	bool GetIsOverrideFOV() { return bIsOverrideFOV; }
	void SetFieldOfView(float newFOV) { FieldOfView = newFOV; }
	```

### ULyraCameraModeStack ->LyraGame/Camera/LyraCameraMode.cpp
- add an FOV override to the PuchCameraMode Method:
	```cpp
    void ULyraCameraModeStack::PushCameraMode(TSubclassOf<ULyraCameraMode> CameraModeClass)
    {
        // ...
        // ...
        // ...

        ULyraCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
        check(CameraMode);

        if (CameraMode->GetIsOverrideFOV())
        {
            const float newFOV = CameraMode->GetLyraCameraComponent()->FieldOfView;
            CameraMode->SetFieldOfView(newFOV);
        }

        int32 StackSize = CameraModeStack.Num();

        // ...
        // ...
        // ...
    }
	```

### ULyraWeaponStateComponent -> LyraGame/Weapons/LyraWeaponStateComponent.h
- export LYRAGAME_API for ULyraWeaponStateComponent

## Installation
Make sure that you have made the changes above to your Lyra classes and then just clone this repo to ```<your-project>/Plugins/GameFeatures```.