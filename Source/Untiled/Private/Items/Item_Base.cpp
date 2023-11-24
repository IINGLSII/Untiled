// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item_Base.h"

// Sets default values
AItem_Base::AItem_Base()
{
	// Add item mesh
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	ItemMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AItem_Base::BeginPlay()
{
	Super::BeginPlay();
}

EGripType AItem_Base::GetGripType()
{
	return GripType;
}

void AItem_Base::Interact_Implementation(AActor* instigator)
{
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UInventory* inventory = Cast<UInventory>(instigator->GetComponentByClass(UInventory::StaticClass()));
	if (inventory) {
		inventory->StoreItem(this);
	}
}

void AItem_Base::Use(ACharacter_Base* caller)
{
	FinishUse();
}

void AItem_Base::AltUse(ACharacter_Base* caller)
{
	FinishUse();
}

void AItem_Base::Drop(ACharacter_Base* caller)
{
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetSimulatePhysics(true);
}

void AItem_Base::FinishUse() {
	ItemFinishDelegate.Execute();
}
