// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Inventory.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();
	InventoryArray.Init(nullptr, InventorySize);
}

AItem_Base* UInventory::GetEquippedItem()
{
	return InventoryArray[0];
}

void UInventory::RemoveItem(uint8 ItemIndex)
{
	AItem_Base* RemovedItem = InventoryArray[ItemIndex];
	if (RemovedItem) {
		RemoveItemHandler.ExecuteIfBound(RemovedItem, ItemIndex);
		InventoryArray[ItemIndex] = NULL;
	}
}

uint8 UInventory::StoreItem(AItem_Base* Item)
{
	for (uint8 i = 0; i < InventorySize; i++) {
		if (!InventoryArray[i]) {
			InventoryArray[i] = Item;
			EquipItemHandler.ExecuteIfBound(Item, i);
			return i;
		}
	}
	EquipItem(Item);
	return 0;
}

void UInventory::SelectItem(uint8 ItemIndex)
{
	AItem_Base* temp = InventoryArray[0];
	InventoryArray[0] = InventoryArray[ItemIndex];
	InventoryArray[ItemIndex] = temp;

	EquipItemHandler.ExecuteIfBound(InventoryArray[0], 0);
	EquipItemHandler.ExecuteIfBound(InventoryArray[ItemIndex], ItemIndex);
	return;
}

void UInventory::EquipItem(AItem_Base* Item)
{
	if (InventoryArray[0])
		RemoveItem();
	InventoryArray[0] = Item;
	EquipItemHandler.ExecuteIfBound(Item, 0);
	return;
}





