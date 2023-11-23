// Fill out your copyright notice in the Description page of Project Settings.


#include "Character System/Character_Base.h"

void ACharacter_Base::Die() {
	GetMesh()->PlayAnimation(DeathAnimation, false);
}

// Returns copy of character info
FCharacterInfo ACharacter_Base::GetCharInfo() const
{
	return CharInfo;
}

// Sets default values
ACharacter_Base::ACharacter_Base()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Decrease character health, will call death if health < 0
void ACharacter_Base::TakeDamage(uint8 Damage)
{
	CharInfo.Health = CharInfo.Health - Damage > 0 ? CharInfo.Health - Damage : 0;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health %d"), CharInfo.Health));
	if (CharInfo.Health)
		return;
	
	Die();
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

// Resets movement speed to default speed
void ACharacter_Base::ResetMovementSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
}

// Called to bind functionality to input
void ACharacter_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Movement input handler
void ACharacter_Base::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
