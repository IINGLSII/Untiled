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
		UPROPERTY()
		int8 damage;

		UPROPERTY(EditDefaultsOnly)
		FAttackInfo attack_info;

		UPROPERTY(EditDefaultsOnly)
		FAttackInfo alt_attack_info;
};

UCLASS()
class AWeapon_Base : public AItem_Base
{
	GENERATED_BODY()

public:
	AWeapon_Base();

protected:
	// information for the weapon
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FWeaponInfo weapon_info;

	// Weapon Mesh
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	UStaticMeshComponent* weapon_mesh;

private:
	// ref to owning character's combat component
	UPROPERTY()
	UCombat* combat_component;

public:
	// Item event, called when using the weapon
	virtual void Use(ACharacter_Base* caller) override;

	// Item event, called when alt using the weapon
	virtual void AltUse(ACharacter_Base* caller) override;

	// Called when a player drops the weapon
	virtual void Drop(ACharacter_Base* caller) override;

	// called on interaction
	virtual void interact_Implementation(AActor* instigator) override;

	// called on weapon hit
	UFUNCTION(BlueprintNativeEvent)
	void on_hit(AActor* hit_actor);

	// C++ implementation
	void on_hit_Implementation(AActor* hit_actor);

	
	
};
