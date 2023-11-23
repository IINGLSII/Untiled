// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"

// FORWARD DECLARATION
class AItem_Base;


DECLARE_DELEGATE_TwoParams(FEquipableDelegate, AItem_Base*, uint8);

UENUM(BlueprintType)
enum class EGripType : uint8 {			// params:
	NONE = 0		UMETA(DisplayName = "NONE"),
	H1 = 1		UMETA(DisplayName = "H1"),
	H2 = 2		UMETA(DisplayName = "H2"),
	OTHER = 3	UMETA(DisplayName = "OTHER")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	

// INTERFACE
	
	// Sets default values for this component's properties
	UInventory();

	// Delegate : Executed when item is equipped
	FEquipableDelegate EquipItemHandler;

	// Delegate : Executed when item is dropped
	FEquipableDelegate RemoveItemHandler;

	// Interface function: returns ref to equipped item
	UFUNCTION(BlueprintCallable)
	AItem_Base* GetEquippedItem();

	// equips given item reference
	UFUNCTION(BlueprintCallable)
	void EquipItem(AItem_Base* item);

	//  drops equipped item
	UFUNCTION(BlueprintCallable)
	void RemoveItem(uint8 ItemIndex = 0);

	// store given item at index
	UFUNCTION(BlueprintCallable)
	uint8 StoreItem(AItem_Base* Item);

	// equip item at a given index in the inventory
	UFUNCTION(BlueprintCallable)
	void SelectItem(uint8 ItemIndex);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Currently equipped item
	UPROPERTY()
	uint8 EquippedItemIndex;

	UPROPERTY(EditDefaultsOnly)
	uint8 InventorySize = 1;

	UPROPERTY()
	TArray<AItem_Base*> InventoryArray;
};

