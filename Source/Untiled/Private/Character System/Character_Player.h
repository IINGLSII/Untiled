// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character System/Character_Base.h"
#include "InputActionValue.h"
#include "Components/Interactive.h"
#include "Components/Combat.h"
#include "Components/Action.h"
#include "Items/Item_Base.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character_Player.generated.h"

/**
 * 
 */

DECLARE_DELEGATE_OneParam(FActionDelegate, const FInputActionValue& Value)

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
		class UInventory* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
		class UAction* ActionManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* InteractionBoundary;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DodgeAnimation;

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
		
	public:
		ACharacter_Player();

		// Inventory management functions
		UFUNCTION()
		void OnEquipItem(AItem_Base* item, uint8 ItemIndex);

		UFUNCTION()
		void OnDropItem(AItem_Base* item, uint8 ItemIndex);

		

	protected:

		/** Called for attack input */
		UFUNCTION()
		void Use(const FInputActionValue& Value);

		/** Called for alt attack input */
		UFUNCTION()
		void AltUse(const FInputActionValue& Value);

		/** Called for dodge input */
		UFUNCTION()
		void Dodge(const FInputActionValue& Value);
		
		/** Called for interact input */
		UFUNCTION()
		void ItemDrop(const FInputActionValue& Value);

		/** Called for left item select input */
		UFUNCTION()
		void LeftItemSelect(const FInputActionValue& Value);

		/** Called for right iten select input */
		UFUNCTION()
		void RightItemSelect(const FInputActionValue& Value);

		/*  Called on attempting to interact with object */
		UFUNCTION()
		void Interact(const FInputActionValue& Value);

		UFUNCTION(Server, Unreliable)
		void PlayMontage(UAnimMontage Animation);

		UFUNCTION(NetMulticast, Unreliable)
		void PlayMontageRPC(UAnimMontage Animation);

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

