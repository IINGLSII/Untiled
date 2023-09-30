// Fill out your copyright notice in the Description page of Project Settings.


#include "Character System/Character_Base.h"

void ACharacter_Base::die() {
	GetMesh()->PlayAnimation(death_animation, false);
}

FCharacterInfo ACharacter_Base::get_char_info() const
{
	return char_info;
}

// Sets default values
ACharacter_Base::ACharacter_Base()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ACharacter_Base::take_damage(uint8 damage)
{
	char_info.health = char_info.health - damage > 0 ? char_info.health - damage : 0;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health %d"), char_info.health));
	if (char_info.health)
		return;
	
	die();
}

// Called when the game starts or when spawned
void ACharacter_Base::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACharacter_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACharacter_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

