// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character System/Character_Base.h"
#include "InputActionValue.h"
#include "../Items/Item_Base.h"
#include "Character_Player.generated.h"

/**
 * 
 */
UCLASS()
class ACharacter_Player : public ACharacter_Base
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Attack Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* UseAction;

	/** Alt Attack Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AltUseAction;

	/** Dodge Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* DodgeAction;

	/** Interact Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;

	/** Item Select Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ItemSelectAction;

	/** Item Drop Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ItemDropAction;

	protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
		class AItem_Base* equipped_item;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
		class AItem_Base* item_slot1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
		class AItem_Base* item_slot2;

	public:
		ACharacter_Player();

	protected:
		/** Called for movement input */
		void Move(const FInputActionValue& Value);

		/** Called for attack input */
		void Use(const FInputActionValue& Value);

		/** Called for alt attack input */
		void AltUse(const FInputActionValue& Value);

		/** Called for dodge input */
		void Dodge(const FInputActionValue& Value);

		/** Called for interact input */
		void Interact(const FInputActionValue& Value);

		/** Called for interact input */
		void ItemSelect(const FInputActionValue& Value);

		/** Called for interact input */
		void ItemDrop(const FInputActionValue& Value);

	protected:
		// APawn interface
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		// To add mapping context
		virtual void BeginPlay() override;

	public:
		/** Returns CameraBoom subobject **/
		FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
		/** Returns FollowCamera subobject **/
		FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

