// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon_Base.h"

AWeapon_Base::AWeapon_Base()
{
	// Add weapon mesh
	weapon_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	weapon_mesh->SetupAttachment(RootComponent);
}

void AWeapon_Base::Use(ACharacter_Base* caller)
{
	// Start attack tracing, bind On-Hit to trace
	if (combat_component) {
		combat_component->attack(weapon_info.attack_info, caller->GetActorLocation());
		combat_component->on_hit_delegate.BindUFunction(this, FName("on_hit"));
	}
}

void AWeapon_Base::AltUse(ACharacter_Base* caller)
{
	// Start attack tracing, bind On-Hit to trace
	if (combat_component) {
		combat_component->attack(weapon_info.alt_attack_info, caller->GetActorLocation());
		combat_component->on_hit_delegate.BindUFunction(this, FName("on_hit"));
	}
}

void AWeapon_Base::Drop(ACharacter_Base* caller)
{
	combat_component = NULL;
}

void AWeapon_Base::interact_Implementation(AActor* instigator)
{
	// pickup and set combat component ref
	Super::interact_Implementation(instigator);
	combat_component = Cast<UCombat>(instigator->GetComponentByClass(UCombat::StaticClass()));
}

void AWeapon_Base::on_hit_Implementation(AActor* hit_actor)
{
	// do damage
	if (hit_actor->Implements<UDamageable>()) {
		IDamageable* damageable_actor = Cast<IDamageable>(hit_actor);
		damageable_actor->take_damage(weapon_info.damage);
	}
	
	// pushback
	if (hit_actor->GetClass()->IsChildOf(ACharacter_Base::StaticClass())) {
		float launch_force = 1000;
		FVector launch_vector = (hit_actor->GetActorLocation() - combat_component->GetOwner()->GetActorLocation()).GetSafeNormal() * launch_force;
		Cast<ACharacter_Base>(hit_actor)->LaunchCharacter(launch_vector, true, false);
	}
}
