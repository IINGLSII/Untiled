// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Interactive.h"

// Sets default values for this component's properties
UInteractive::UInteractive()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UInteractive::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
}

// Setup Interactable Boundary
void UInteractive::SetupCollision(UCapsuleComponent* collision) {
	collision->SetCollisionProfileName("Trigger");
	collision->OnComponentBeginOverlap.AddDynamic(this, &UInteractive::InteractBoundaryOverlapBegin);
	collision->OnComponentEndOverlap.AddDynamic(this, &UInteractive::InteractBoundaryOverlapEnd);
}

// Remove Interactable Object From Set
void UInteractive::removeInteractable(IInteractable* object)
{
	verify(interactables.Contains(object));

	interactables.Remove(object);
	// if set is not empty, set interact object to first item in set
	if (!interactables.IsEmpty())
		interact_object = *interactables.begin();
	else
		interact_object = NULL;
}

// Add Interactable Object To Set
void UInteractive::addInteractable(IInteractable* object)
{
	interactables.Add(object);
	// set interact object to most recently added object
	if (!interact_object)
		interact_object = object;
}

// Command to interact with object
void UInteractive::interact()
{
	if (interact_object) {
		IInteractable::Execute_interact(Cast<UObject>(interact_object), Cast<ACharacter_Base>(GetOwner()));
		removeInteractable(interact_object);
	}
}

// Interactable Enters Boundary
void UInteractive::InteractBoundaryOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UInteractable>()) {
		IInteractable* interactable_object = Cast<IInteractable>(OtherActor);
		UInteractive::addInteractable(interactable_object);
	}
}

// Interactable Leaves Boundary
void UInteractive::InteractBoundaryOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->Implements<UInteractable>()) {
		IInteractable* interactable_object = Cast<IInteractable>(OtherActor);
		if (interactables.Contains(interactable_object))
			removeInteractable(interactable_object);
	}
}


// Called every frame
void UInteractive::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

