// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item_Base.h"

// Sets default values
AItem_Base::AItem_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

EGripType AItem_Base::get_grip_type()
{
	return grip_type;
}

// Called every frame
void AItem_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem_Base::interact_Implementation(AActor* instigator)
{
	UEquipable* inventory = Cast<UEquipable>(instigator->GetComponentByClass(UEquipable::StaticClass()));
	if (inventory) {
		inventory->equip_item(this);
	}
	inventory->set_grip_type(grip_type);
	
}

void AItem_Base::Use(ACharacter_Base* caller)
{

}

void AItem_Base::AltUse(ACharacter_Base* caller)
{

}

void AItem_Base::Drop(ACharacter_Base* caller)
{
}

