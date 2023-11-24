// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon_Base.h"

AWeapon_Base::AWeapon_Base()
{
}

void AWeapon_Base::LaunchTarget(AActor* target, float x_force, float y_force)
{
	if (target->GetClass()->IsChildOf(ACharacter_Base::StaticClass())) {
		FVector launch_vector = (target->GetActorLocation() - CombatComponent->GetOwner()->GetActorLocation()).GetSafeNormal()*x_force + FVector(0,0,y_force);
		Cast<ACharacter_Base>(target)->LaunchCharacter(launch_vector, true, false);
	}
	return;
}

void AWeapon_Base::Attack(TArray<FAttackInfo>* AttackInfo, AActor* attacker)
{
	if (CombatComponent && !(AttackInfo->IsEmpty())) {
		ComboCounter %= AttackInfo->Num();
		CombatComponent->OnHitDelegate.BindUFunction(this, FName("OnHit"));
		CombatComponent->AttackResetDelegate.BindUFunction(this, FName("FinishUse"));
		CombatComponent->AttackTrace((*AttackInfo)[ComboCounter], attacker->GetActorLocation());
		ComboCounter++;
	}
	else {
		FinishUse();
	}
	
}

void AWeapon_Base::Use(ACharacter_Base* caller)
{
	Attack(&WeaponInfo.AttackInfo, caller);
}

void AWeapon_Base::AltUse(ACharacter_Base* caller)
{
	Attack(&WeaponInfo.AltAttackInfo, caller);
}

void AWeapon_Base::Drop(ACharacter_Base* caller)
{
	CombatComponent = NULL;
	Super::Drop(caller);
}

void AWeapon_Base::Interact_Implementation(AActor* instigator)
{
	// pickup and set combat component ref
	Super::Interact_Implementation(instigator);
	CombatComponent = Cast<UCombat>(instigator->GetComponentByClass(UCombat::StaticClass()));
}

void AWeapon_Base::OnHit_Implementation(AActor* hit_actor)
{
	// do damage
	if (hit_actor->Implements<UDamageable>()) {
		IDamageable* damageable_actor = Cast<IDamageable>(hit_actor);
		damageable_actor->TakeDamage(WeaponInfo.Damage);
	}
	
	// pushback
	LaunchTarget(hit_actor, 1000, 0);
}
