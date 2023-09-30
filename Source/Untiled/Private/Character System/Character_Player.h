// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character System/Character_Base.h"
#include "InputActionValue.h"
#include "Components/Interactive.h"
#include "Components/Combat.h"
#include "Items/Item_Base.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
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

	/** Interact Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
		class UInteractive* Interactive;

	/** Interact Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
		class UCombat* CombatComponent;

	/** Interact Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
		class UEquipable* Equipment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* InteractionBoundary;

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

	/** Left Item Select Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LeftItemSelectAction;

	/** Right Item Select Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RightItemSelectAction;

	/** Item Drop Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ItemDropAction;

	protected:
	/* Item stored in item slot 1*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
		AItem_Base* item_slot1;

	/* Item stored in item slot 2*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
		AItem_Base* item_slot2;

	public:
		ACharacter_Player();

		// Delegate functions from Inventory
		UFUNCTION()
		void equip_item(AItem_Base* item);

		UFUNCTION()
		void drop_item(AItem_Base* item);

	protected:
		/** Called for movement input */
		void Move(const FInputActionValue& Value);

		/** Called for attack input */
		void Use(const FInputActionValue& Value);

		/** Called for alt attack input */
		void AltUse(const FInputActionValue& Value);

		/** Called for dodge input */
		void Dodge(const FInputActionValue& Value);

		/** Called for left item select input */
		void LeftItemSelect(const FInputActionValue& Value);

		/** Called for right iten select input */
		void RightItemSelect(const FInputActionValue& Value);

		/** Called for interact input */
		void ItemDrop(const FInputActionValue& Value);


	protected:
		// APawn interface
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		// To add mapping context
		virtual void BeginPlay() override;

		// Called when any item is selected
		void ItemSelect(AItem_Base** item, FName socketName);

	private:
		

	public:
		/** Returns CameraBoom subobject **/
		FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
		/** Returns FollowCamera subobject **/
		FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

