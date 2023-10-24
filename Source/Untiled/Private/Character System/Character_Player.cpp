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
	Interactive->SetupCollision(InteractionBoundary);

	// Add Combat Component (Tracing)
	CombatComponent = CreateDefaultSubobject<UCombat>(TEXT("CombatComponent"));

	// Equipment Component (Ablt to equip items)
	Equipment = CreateDefaultSubobject<UEquipable>(TEXT("Equipment"));
	Equipment->equip_item_delegate.BindUFunction(this, FName("equip_item"));
	Equipment->drop_item_delegate.BindUFunction(this, FName("drop_item"));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	
	GetCharacterMovement()->MaxWalkSpeed = base_movement_speed;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ACharacter_Player::equip_item(AItem_Base* item)
{
	// if item is not null, attach to equip socket
	if (item) 
		item->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("equipSocket"));
}

void ACharacter_Player::drop_item(AItem_Base* item)
{
	if (item) {
		item->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	}
}

void ACharacter_Player::Use(const FInputActionValue& Value)
{
	// use equipped item
	AItem_Base* item = Equipment->get_equipped_item();
	if (item)
		item->Use(this);
}

void ACharacter_Player::AltUse(const FInputActionValue& Value)
{
	// use alternate usage for item
	AItem_Base* item = Equipment->get_equipped_item();
	if (item)
		item->AltUse(this);
}

void ACharacter_Player::Dodge(const FInputActionValue& Value)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Dodge")));
	SetActorRotation(GetCharacterMovement()->GetLastInputVector().Rotation());
	PlayAnimMontage(dodge_animation);
}

void ACharacter_Player::LeftItemSelect(const FInputActionValue& Value) { ACharacter_Player::ItemSelect(&item_slot1, FName("item1Socket"));
}

void ACharacter_Player::RightItemSelect(const FInputActionValue& Value) { ACharacter_Player::ItemSelect(&item_slot2, FName("item2Socket"));
}

void ACharacter_Player::ItemSelect(AItem_Base** item, FName socketName)
{
	// swap new and old item references
	AItem_Base* new_equipped_item = *item;
	AItem_Base* old_equipped_item = Equipment->get_equipped_item();
	Equipment->equip_item(new_equipped_item);
	*item = old_equipped_item;
	
	// if item is not null, attach old item to character belt
	if(old_equipped_item)
		old_equipped_item->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), socketName);
	// if new item is not null, attach to equip socket (hands)
	if (new_equipped_item) {
		new_equipped_item->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("equipSocket"));
		Equipment->set_grip_type(new_equipped_item->get_grip_type());
	}
}

void ACharacter_Player::ItemDrop(const FInputActionValue& Value) { Equipment->drop_equipped_item(); }

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
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, Interactive, &UInteractive::interact);

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


