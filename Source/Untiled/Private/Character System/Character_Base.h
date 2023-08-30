// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interfaces/Damageable.h"
#include "Character_Base.generated.h"

USTRUCT(BlueprintType)
struct FCharacterInfo {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	uint8 health;

	UPROPERTY(BlueprintReadOnly)
	FString name;
};

UCLASS()
class ACharacter_Base : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter_Base();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterSystem, meta = (AllowPrivateAccess = "true"))
	FCharacterInfo char_info;

	UPROPERTY(EditDefaultsOnly, Category = CharacterSystem, meta = (AllowPrivateAccess = "true"))
	class UAnimationAsset* death_animation;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay();

	// pure virtual : called when health is reduced to 0
	virtual void die();

public:
	// returns copy of char_info
	UFUNCTION(BlueprintCallable)
	FCharacterInfo get_char_info() const;

	// interface : called on taking a hit
	virtual void take_damage(uint8) override;

	// Called every frame
	virtual void Tick(float DeltaTime);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

};
