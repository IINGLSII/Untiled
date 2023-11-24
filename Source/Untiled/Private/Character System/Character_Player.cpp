// Fill out your copyright notice in the Description page of Project Settings.


#include "Character System/Character_Player.h"

ACharacter_Player::ACharacter_Player() {
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create Interaction Component and Interaction Collision
	Interactive = CreateDefaultSubobject<UInteractive>(TEXT("Interactive"));
	InteractionBoundary = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionBoundary"));
	InteractionBoundary->SetupAttachment(RootComponent);
	Interactive->SetCollision(InteractionBoundary);

	// Add Combat Component (Tracing)
	CombatComponent = CreateDefaultSubobject<UCombat>(TEXT("CombatComponent"));

	// Equipment Component (Ablt to equip items)
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
	Inventory->EquipItemHandler.BindUFunction(this, FName("OnEquipItem"));
	Inventory->RemoveItemHandler.BindUFunction(this, FName("OnDropItem"));

	ActionManager = CreateDefaultSubobject <UAction> (TEXT("ActionManager"));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

// called when an item is equipped: attaches object to player
void ACharacter_Player::OnEquipItem(AItem_Base* Item, uint8 index)
{
	// if item is not null, attach to equip socket
	if (Item) {
		Item->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName(FString::Printf(TEXT("InventorySocket%d"), index)));
		Item->ItemFinishDelegate.BindUFunction(ActionManager, FName("FinishAction"));
	}
}

// called when item is dropped: detaches item from actor
void ACharacter_Player::OnDropItem(AItem_Base* Item, uint8 index)
{
	if (Item) {
		Item->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
		Item->Drop(this);
	}
}

// Input Action Event: called when player attempts to 'use' an item
void ACharacter_Player::Use(const FInputActionValue& Value)
{
	AItem_Base* item = Inventory->GetEquippedItem();
	if (item && ActionManager->StartAction(FName("Use"), Value)) {
		item->Use(this);
	}		
}

// Input Action Event: called when player attempts to 'altuse' an item
void ACharacter_Player::AltUse(const FInputActionValue& Value)
{
	AItem_Base* item = Inventory->GetEquippedItem();
	if (item && ActionManager->StartAction(FName("AltUse"), Value)) {
		item->AltUse(this);
	}
}

//// Input Action Event: called when player attempts to dodge
void ACharacter_Player::Dodge(const FInputActionValue& Value)
{
	if (ActionManager->StartAction(FName("Dodge"), Value)) {
		SetActorRotation(GetCharacterMovement()->GetLastInputVector().Rotation());
		PlayAnimMontage(DodgeAnimation);
		ActionManager->FinishActionTimer(0.5);
	}
}

// Input Action Event: called when player wants to equip item in left slot
void ACharacter_Player::LeftItemSelect(const FInputActionValue& Value) {
	if (ActionManager->StartAction(FName("LeftItemSelect"), Value)) {
		Inventory->SelectItem(1);
		ActionManager->FinishAction();
	}
}

// Input Action Event: called when player wants to equip item in right slot
void ACharacter_Player::RightItemSelect(const FInputActionValue& Value) { 
	if (ActionManager->StartAction(FName("RightItemSelect"), Value)) {
		Inventory->SelectItem(2);
		ActionManager->FinishAction();
	}
}

void ACharacter_Player::Interact(const FInputActionValue& Value)
{
	if (ActionManager->StartAction(FName("Interact"), Value)) {
		Interactive->Interact();
		ActionManager->FinishAction();
	}
}

// Input Action Event: called when player attempts to drop currently equipped item
void ACharacter_Player::ItemDrop(const FInputActionValue& Value) {
	if (ActionManager->StartAction(FName("ItemDrop"), Value)) {
		Inventory->RemoveItem();
		ActionManager->FinishAction();
	}
}

void ACharacter_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacter_Player::Move);

		//Attack
		EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Triggered, this, &ACharacter_Player::Use);

		//Alt Attacking
		EnhancedInputComponent->BindAction(AltUseAction, ETriggerEvent::Triggered, this, &ACharacter_Player::AltUse);

		//Dodge
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ACharacter_Player::Dodge);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ACharacter_Player::Interact);

		//Left Item Select
		EnhancedInputComponent->BindAction(LeftItemSelectAction, ETriggerEvent::Triggered, this, &ACharacter_Player::LeftItemSelect);

		//Right Item Select
		EnhancedInputComponent->BindAction(RightItemSelectAction, ETriggerEvent::Triggered, this, &ACharacter_Player::RightItemSelect);

		//Item Drop
		EnhancedInputComponent->BindAction(ItemDropAction, ETriggerEvent::Triggered, this, &ACharacter_Player::ItemDrop);
	}

}

void ACharacter_Player::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context


	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


