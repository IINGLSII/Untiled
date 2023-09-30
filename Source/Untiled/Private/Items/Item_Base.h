// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/Interactable.h"
#include "components/Equipable.h"
#include "Item_Base.generated.h"

UCLASS()
class AItem_Base : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	EGripType grip_type;

public:	

	// returns grip type of the weapon
	EGripType get_grip_type();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when a player uses an item
	virtual void Use(ACharacter_Base* caller);

	// Called when a player alt uses an item
	virtual void AltUse(ACharacter_Base* caller);

	// Called when a player drops the item
	virtual void Drop(ACharacter_Base* caller);

	// called on interaction
	virtual void interact_Implementation(AActor* instigator) override;
};
