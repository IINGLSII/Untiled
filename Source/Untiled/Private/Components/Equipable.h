// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Equipable.generated.h"

// FORWARD DECLARATION
class AItem_Base;

DECLARE_DELEGATE_OneParam(FEquipableDelegate, AItem_Base*);

UENUM(BlueprintType)
enum class EGripType : uint8 {			// params:
	HandsDown = 0		UMETA(DisplayName = "HandsDown"),
	OneHanded = 1		UMETA(DisplayName = "OneHanded"),
	TwoHanded = 2		UMETA(DisplayName = "TwoHanded"),
	OneHandedUtil = 3	UMETA(DisplayName = "OneHandedUtil"),
	TwoHandedUtil = 4	UMETA(DisplayName = "TwoHandedUtil")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEquipable : public UActorComponent
{
	GENERATED_BODY()


public:	
	
	// Sets default values for this component's properties
	UEquipable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Currently equipped item
	UPROPERTY()
	AItem_Base* equipped_item;

	// How the weapon is held
	UPROPERTY()
	EGripType grip_type;

public:	

	// Interface function: returns ref to equipped item
	UFUNCTION(BlueprintCallable)
	AItem_Base* get_equipped_item();

	// Delegate : Executed when item is equipped
	FEquipableDelegate equip_item_delegate;

	// Delegate : Executed when item is dropped
	FEquipableDelegate drop_item_delegate;

	// Interface function: drops equipped item
	void equip_item(AItem_Base* item);

	void set_grip_type(EGripType);

	UFUNCTION(BlueprintCallable)
	void drop_equipped_item();

	UFUNCTION(BlueprintCallable)
	EGripType get_grip_type();
};

