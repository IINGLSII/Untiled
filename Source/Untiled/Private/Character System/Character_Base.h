// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interfaces/Damageable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character_Base.generated.h"

USTRUCT(BlueprintType)
struct FCharacterInfo {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = CharacterInfo)
	uint8 health;

	UPROPERTY(EditDefaultsOnly, Category = CharacterInfo)
	FString name;
};

UCLASS()
class ACharacter_Base : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter_Base();
	
	// 
protected:

	UPROPERTY(EditDefaultsOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	FCharacterInfo char_info;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	class UAnimationAsset* death_animation;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	float base_movement_speed = 300;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay();

	// pure virtual : called when health is reduced to 0
	virtual void die();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

public:
	// returns copy of char_info
	UFUNCTION(BlueprintCallable)
	FCharacterInfo get_char_info() const;

	// interface : called on taking a hit
	virtual void take_damage(uint8) override;

	// Called every frame
	virtual void Tick(float DeltaTime);

	void reset_movement_speed();

};
