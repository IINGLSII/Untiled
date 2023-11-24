// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/Interactable.h"
#include "components/Inventory.h"
#include "Item_Base.generated.h"

DECLARE_DELEGATE(FItemUseFinishDelegate)

UCLASS()
class AItem_Base : public AActor, public IInteractable
{
	GENERATED_BODY()
	
//INTERFACE
public:	

// INTERACTABLE OVERRIDES
	
	// called on interaction
	virtual void Interact_Implementation(AActor* instigator);

// NEW METHODS
 
	// Sets default values for this actor's properties
	AItem_Base();

	// Delegate Handler
	FItemUseFinishDelegate ItemFinishDelegate;

	// returns grip type of the weapon
	EGripType GetGripType();

	// Called when a player uses an item
	virtual void Use(ACharacter_Base* caller);

	// Called when a player alt uses an item
	virtual void AltUse(ACharacter_Base* caller);

	// Called when a player drops the item
	virtual void Drop(ACharacter_Base* caller);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void FinishUse();

	UPROPERTY(EditDefaultsOnly, Category = Item)
	EGripType GripType;

	// item mesh
	UPROPERTY(EditDefaultsOnly, Category = Item)
	UStaticMeshComponent* ItemMesh;
};
