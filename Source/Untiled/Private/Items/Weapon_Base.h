// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item_Base.h"
#include "Components/Combat.h"
#include "Weapon_Base.generated.h"

USTRUCT(BlueprintType)
struct FWeaponInfo {
	GENERATED_BODY()

	public:
		UPROPERTY(EditDefaultsOnly)
		int8 Damage;

		UPROPERTY(EditDefaultsOnly)
		TArray<FAttackInfo> AttackInfo;

		UPROPERTY(EditDefaultsOnly)
		TArray<FAttackInfo> AltAttackInfo;
};

UCLASS()
class AWeapon_Base : public AItem_Base
{
	GENERATED_BODY()


// INTERFACE
public:
	AWeapon_Base();

// ITEM SUBCLASS OVERRIDES
	
	// item event, called when using the weapon
	virtual void Use(ACharacter_Base* caller) override;

	// item event, called when alt using the weapon
	virtual void AltUse(ACharacter_Base* caller) override;

	// called when a player drops the weapon
	virtual void Drop(ACharacter_Base* caller) override;

	// called on weapon pickup
	virtual void Interact_Implementation(AActor* instigator) override;

// NEW METHODS
	
	// called on weapon hit
	UFUNCTION(BlueprintNativeEvent)
	void OnHit(AActor* hit_actor);

	// called on weapon hit
	virtual void OnHit_Implementation(AActor* hit_actor);

// DEFAULT PARAMs AND COMPONENTS
protected:
	// information for the weapon
	UPROPERTY(EditDefaultsOnly, Category = Item)
	FWeaponInfo WeaponInfo;

private:
	// ref to owning character's combat component
	UPROPERTY()
	UCombat* CombatComponent;

	// counter representing combo progression
	UPROPERTY()
	uint8 ComboCounter = 0;

// UTILITY FUNCTIONS
private:
	void Attack(TArray<FAttackInfo>* AttackInfo, AActor* attacker);

protected:
	UFUNCTION()
	void LaunchTarget(AActor* target, float x_force, float y_force);
};
