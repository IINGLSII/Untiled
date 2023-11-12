// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Equipable.h"

// Sets default values for this component's properties
UEquipable::UEquipable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UEquipable::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

AItem_Base* UEquipable::get_equipped_item()
{
	return equipped_item;
}

void UEquipable::drop_equipped_item()
{
	drop_item_delegate.ExecuteIfBound(equipped_item);
	equipped_item = NULL;
	set_grip_type(EGripType::NONE);
}

void UEquipable::equip_item(AItem_Base* item)
{
	if (equipped_item)
		drop_equipped_item();
	equipped_item = item;
	equip_item_delegate.ExecuteIfBound(item);
}

EGripType UEquipable::get_grip_type()
{
	return grip_type;
}

void UEquipable::set_grip_type(EGripType new_grip_type)
{
	grip_type = new_grip_type;
}





